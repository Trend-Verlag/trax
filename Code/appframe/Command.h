//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2014 
//
// "Niemand hat das Recht zu gehorchen"
//
//				Hannah Arendt

#pragma once

#include "AppFrame.h"

#include <queue>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <cassert>


/// \brief The namespace provides an implementation of the command pattern.
namespace cmnd{

	class Command;

	/// \brief A logger that can get attached to every container of commands like
	/// Macro or History.
	class Logbook{
	public:
		void operator()( const Command& command ){
			Log( command );
		}

		/// \brief Log a command.
		virtual void Log( const Command& command ) = 0;

		virtual ~Logbook() = default;

		Logbook() = default;
		Logbook( const Logbook& ) = default;
		Logbook( Logbook&& ) = default;
		Logbook& operator=( const Logbook& ) = default;
		Logbook& operator=( Logbook&& ) = default;
	};


	/// \brief Innocently captured commands. But they'll be free again!
	///
	/// Everything the engine can do is programmed in the form of a command, implementing the
	/// Command interface. Such a command knows how to execute and knows how to unexecute 
	/// (reverting its effect). This offers a couple of possibilities like Undo/Redo chains,
	/// logging, macros, command mapping (e.g. for Keyboard keys), scripting, feeding progress bars etc pp...
	class dclspc_appframe Command{
	public:
		virtual ~Command() = default;

		Command( const Command& ) = default;
		Command( Command&& ) = default;
		Command& operator=( const Command& ) = delete;
		Command& operator=( Command&& ) = delete;

		void* operator new  (std::size_t n)     { return dll_alloc(n); }
		void  operator delete(void* p) noexcept { dll_free(p); }


		/// \brief Operator to execute the command.
		void operator()(){
			Execute();
		}


		/// \brief Executes the command.
		virtual void Execute() = 0;


		/// \brief Reverses the effect of a command. 
		/// The command has to be executed first.
		///
		/// The default behaviour of the Command implementation is
		/// to assume two times of execution yields original state.
		virtual void Unexecute(){
			Execute();
		}


		/// \brief Name of the command call.
		virtual const char* Name() const;


		/// \returns a valid call in text form to execute this command. 
		/// Memory is valid until the next call to Call() on this thread.
		virtual const char* Call() const;


		/// \brief Reads the Command's parameter list from an input stream.
		void ParameterFrom( std::istream& istream );


		/// \brief Sets the time stamp for this command.
		virtual void TimeStamp( long time );


		/// \brief Gets the time stamp for this command.
		long TimeStamp() const noexcept;


		/// \brief Sets the time stamp of the macro and the time steps of
		/// the commands to the same value.
		/// \param time time step to set to.
		virtual void SetAllTimeStamps( long time );


		/// \brief Request the command to fix it's state.
		///
		/// Some commands rely on dynamic data that changes with time.
		/// If the command is needed with the data at a specific
		/// moment, this method can be called to freeze the data
		/// as is at the moment of the call.
		virtual void Freeze() noexcept;


		/// \returns true if the comand was frozen by the Freeze call.
		bool IsFrozen() const noexcept;


		/// \brief Make a copy of this command. 
		virtual std::unique_ptr<Command> Clone() const = 0;


		/// \brief Is the command valid in an executable state.
		/// \returns true if the command is valid; false otherwise.
		virtual bool Valid() const noexcept { return true; }


		/// \brief Unknown method.
		virtual bool Profound() const noexcept { return true; }


		/// \brief is the command reversible.
		/// \returns true if the command can get unexecuted after execution.
		virtual bool Reversible() const noexcept { return true; }


		/// \returns true if the command is targeting the simulation 
		/// and thereby potentially influences its outcome; false if it is
		/// granted that the simulation will not be influenced.
		virtual bool InfluencesSimulationResult() const noexcept { return true; }


		/// \returns an estimated time duration in milliseconds to execute this
		/// comman.
		virtual unsigned int EstimateExecutionTime() const noexcept { return 1; }


		/// \brief  If a series of Commands is cloned, this method is called, 
		/// to give the Command a chance to remap other Commands if it happens 
		/// to reference them.
		//virtual void MapCommand( const std::pair<std::shared_ptr<Command>,std::shared_ptr<Command>>& /*pFromTo*/ ) {}
	protected:
		Command() noexcept
			:	m_TimeStamp	{0},
				m_bFrozen	{false}
		{}

		/// \brief Override this method so that it reads the parameter list.
		virtual void ReadParameterList( std::istream& istream ){}

		/// \brief Reads a comma separator from an input stream.
		static inline std::istream& separator( std::istream& stream ){
			char c;
			stream >> c;
			if( c != ',' ){
				stream.putback( c );
				throw std::runtime_error{ "Separator ',' not found! Maybe separator was read with string by not using >> Separator{string}?" };
			}

			return stream;
		}

		class readstring{
		public:
			readstring() = delete;
			readstring( std::string& string ) noexcept
				: m_pString{ &string }
			{}

			friend inline std::basic_istream<char>& operator >> ( std::basic_istream<char>& is, const readstring& sep ){
				is >> *sep.m_pString;

				const std::size_t pos = sep.m_pString->find( ',' );
				if( pos != std::string::npos ){
					for( std::size_t i = sep.m_pString->size() - 1; i >= pos; --i ){
						is.putback( sep.m_pString->at( i ) ); //necessary to put back the speperator, since ( string , othervalue ) 
						sep.m_pString->pop_back();
					}
				}
				else if( sep.m_pString->back() == ')' ){
					is.putback( ')' );
					sep.m_pString->pop_back();
				}

				return is;
			}
		private:
			std::string* m_pString;//must be pointer.
		};

		static void* dll_alloc( std::size_t n );
		static void dll_free( void* p ) noexcept;
	private:
		long	m_TimeStamp;
		bool	m_bFrozen;
	};


	/// \brief A Macro holds a list of commands and allows to execute them all at once.
	///
	/// For example an application file might consist of (or get translated to) commands, being
	/// effectively a script file. The file reader would provide a Marco as an output. Now the 
	/// application can make use of Macro::EstimateExecutionTime() to inform the user about the
	/// loading time and feed a status bar via a Logbook - derived object.
	class Macro : public virtual Command{
	public:
		static dclspc_appframe const Macro* Cast( const Command* command ) noexcept;
		static dclspc_appframe Macro* Cast( Command* command ) noexcept;

		/// \name Construction
		/// \param macro Other macro to clone the commands from.
		//@{
		Macro() = default;
		Macro( const Macro& macro );
		Macro( Macro&& macro ) noexcept;
		//@}

		~Macro();

		Macro& operator=( const Macro& macro ) = delete;
		Macro& operator=( Macro&& macro ) noexcept = delete;


		/// \brief Clone the commands from macro and append them to this Macro.
		void operator+=( const Macro& macro );


		/// \brief Append a command to this Macro.
		void Push( std::unique_ptr<Command> pCommand );


		/// \brief Clear the Macro from all commands.
		void Clear();


		/// \brief Exchanges the content between the two macros.
		void Swap( Macro& macro ) noexcept;


		/// \returns true if there are no command in this Macro.
		bool Empty() const noexcept;


		/// \name Removing of commands from stack
		/// \brief Gets a command from the front of the macro if it's 
		/// timestamp is expired
		///
		/// It is assumed that the commands are ordered with time stamp.
		/// \param tillTimeStamp time (typically im milliseconds).
		//@{
		std::unique_ptr<Command> PopFront();

		std::unique_ptr<Command> PopFront( long tillTimeStamp );
		//@}


		/// \brief Removes all commands of a certain type from the list,
		/// preserving the order of the rest.
		template<typename CommandType>
		void Remove();


		/// \brief Executes the commands that have a lower time stamp and 
		/// removes them from the list.
		///
		/// It is assumed that the commands are ordered with time stamp.
		/// \param tillTimeStamp time (typically im milliseconds).
		void Execute( long tillTimeStamp );


		// Command:

		/// \brief Executes the complete Macro.
		void Execute() override;


		/// \brief Unexecutes the complete Macro.
		void Unexecute() override;


		/// \brief Name of the command call.
		const char* Name() const override;


		using Command::TimeStamp;

		/// \brief Sets the time stamp of the first command to time and
		/// recalculates the time stamps of all the command, so the relative
		/// distances stay the same.
		void TimeStamp( long time ) override;


		/// \Sets the time stamp of the macro and the time steps of
		/// the commands to the same value.
		/// \param time time step to set to.
		void SetAllTimeStamps( long time ) override;


		bool Valid		() const noexcept override;
		bool Profound	() const noexcept override;
		bool Reversible	() const noexcept override;

		std::unique_ptr<Command> Clone() const override;

		unsigned int EstimateExecutionTime() const noexcept override;

		/// \brief Logs the commands from this Macro
		void Log( Logbook& log ) const;

		void RegisterCommand( std::unique_ptr<Command> pCommand );

		std::unique_ptr<Command> CommandFrom( const std::string& name ) const;
	private:
		template<typename T>
		struct DllAllocator {
			using value_type = T;

			DllAllocator() = default;

			// Rebind copy constructor:
			template<typename U>
			DllAllocator( const DllAllocator<U>& ) noexcept {}

			T* allocate( std::size_t n )
			{ return static_cast<T*>(dll_alloc(n * sizeof(T))); }

			void deallocate( T* p, std::size_t ) noexcept 
			{ dll_free(p); }

			// Required for container swap/move — stateless allocator, always equal:
			template<typename U>
			friend bool operator==( const DllAllocator&, const DllAllocator<U>& ) noexcept { return true; }

			template<typename U>
			friend bool operator!=( const DllAllocator&, const DllAllocator<U>& ) noexcept { return false; }
		};

		std::deque<Command*,DllAllocator<Command*>> m_Commands;	// can not have a unique_ptr in a container
																// because of copying of elements, albeit unique would
																// be more appropriate. This would work with 
																// std::vector since it is move - aware.

		std::map<const std::string,std::shared_ptr<Command>,std::less<std::string>,
				DllAllocator<std::pair<const std::string, std::shared_ptr<Command>>>> m_RegisteredCommads;

		void _PopFront();
	};

	inline std::ostream& operator<<( std::ostream& ostream, const Macro& macro );

	inline std::istream& operator>>( std::istream& istream, Macro& macro );

	inline std::istream& ReadCommandFrom( std::istream& istream, Macro& macro );


	/// \brief An Undo/Redo chain.
	///
	/// Assuming you maintain a set of command originals for input mapping, to 
	/// use a History, a command gets cloned before executing. The executed command 
	/// is then added to the History. The deletion of e.g. a track will not be 
	/// implemented as actual destruction of the trax object, but as a Remove from its
	/// Tracksystem and kept as an object in the command. If the simple rule is obeyed that
	/// nothing living in the system can reference the objects still held by the commands
	/// a Undo/Redo chain should work pretty well.
	/// 
	/// A complete History of what the user did since program start 
	/// can get saved (maybe in form of script source) and reexecuted at another time on 
	/// another machine  e.g. to reproduce a bug.
	class History{
	public:

		/// \name Construction
		/// \param history
		//@{
		History() noexcept;
		History( const History& history ) noexcept;
		History( History&& history ) noexcept;
		//@}

		virtual ~History() noexcept;


		/// \name Assignment
		//@{
		History& operator=( const History& history ) noexcept;
		History& operator=( History&& history ) noexcept;
		//@}


		void Undo();
		void Redo();

		bool GotUndoCommand() const;
		bool GotRedoCommand() const;
		bool Empty() const noexcept;


		/// \brief Executes a command and adds it to the history. 
		/// 
		/// The command is expected to be cloned before executing, so the 
		/// history has its own copy of the command.
		/// \param pCommand command to execute and add to history.
		void Execute( std::unique_ptr<Command>& pCommand );


		/// \brief Adds a command to the history without executing it.
		void Add( std::unique_ptr<Command>& pCommand );


		/// \brief Clears the history from all commands.
		void Clear() noexcept;

		std::unique_ptr<cmnd::Macro> Macro() const noexcept;
	private:
		using listtype = std::vector<std::shared_ptr<Command>>;
		listtype			m_CommandList;
		listtype::iterator	m_OnePastExecuted;
	};


	/// \brief A LogBook that streams to an std::ostream.
	class OutStreamLog : public Logbook{
	public:
		OutStreamLog( std::ostream& ostr ) noexcept
			:	m_Ostr(ostr)
		{}

		OutStreamLog& operator=( const OutStreamLog& ) = delete; //no assignment

		/// \brief Log a command.
		void Log( const Command& command ) override;

	private:
		std::ostream& m_Ostr;
	};


	/// \brief Base class for executers that might want to perform additional 
	/// actions to executing a command. 
	///
	/// E.g. clone the command, execute the clone and move it to a History
	/// for Undo/Redo functionality. With GUI libraries there might
	/// be the necessity to provide event handlers of certain signature.
	/// These might derive from Executer.
	class Executer{
	public:
		Executer( std::shared_ptr<Command> pCommand ) noexcept
			: m_pCommand(pCommand)
		{}

		void operator()(){
			Execute();
		}

		void Execute(){
			m_pCommand->Execute();
		}

	private:
		std::shared_ptr<Command> m_pCommand;
	};


///////////////////////////////////////
inline Macro::Macro( const Macro& macro )
{
	operator+=(macro);
}

inline Macro::Macro( Macro&& macro ) noexcept
	:	m_Commands			{std::move(macro.m_Commands)},
		m_RegisteredCommads	{std::move(macro.m_RegisteredCommads)}
{}

inline Macro::~Macro(){
	Clear();
}

inline void Macro::operator+=( const Macro& macro ){
	for( auto pCommand : macro.m_Commands )
		Push(pCommand->Clone());
}

inline void Macro::Push( std::unique_ptr<Command> pCommand ){
	if( pCommand ){
		if( m_Commands.empty() )
			// Macro carries timestamp of its first entry
			Command::TimeStamp( pCommand->TimeStamp() );

		m_Commands.push_back( pCommand.release() );
	}
}

inline void Macro::Clear(){
	for( auto pCommand : m_Commands )
		delete pCommand;

	m_Commands.clear();
}

inline void Macro::Swap( Macro& macro ) noexcept{
	std::swap( m_Commands, macro.m_Commands );
}

inline bool Macro::Empty() const noexcept{
	return m_Commands.empty();
}

inline std::unique_ptr<Command> Macro::PopFront(){
	if( !m_Commands.empty() )
	{
		std::unique_ptr<Command> retval( m_Commands.front() );
		_PopFront();
		return retval;
	}

	return nullptr;
}

inline std::unique_ptr<Command> Macro::PopFront( long tillTimeStamp ){
	if( !m_Commands.empty() &&
		m_Commands.front()->TimeStamp() <= tillTimeStamp )
	{
		std::unique_ptr<Command> retval( m_Commands.front() );
		_PopFront();
		return retval;
	}

	return nullptr;
}

template<typename CommandType> inline
void Macro::Remove(){
	auto iter = m_Commands.begin();
	while( iter != m_Commands.end() ){
		if( dynamic_cast<CommandType*>(*iter) ){
			delete *iter;
			iter = m_Commands.erase( iter );
		}
		else
			++iter;
	}
}

inline void Macro::Execute( long tillTimeStamp ){
	while( !m_Commands.empty() &&
			m_Commands.front()->TimeStamp() <= tillTimeStamp )
	{
		m_Commands.front()->Execute();
		delete m_Commands.front();
		_PopFront();
	}
}

inline void Macro::Execute(){
	for( auto pCommand : m_Commands )
		pCommand->Execute();
}

inline void Macro::Unexecute(){
	for( auto pCommand : m_Commands )
		pCommand->Unexecute();
}

inline const char* Macro::Name() const{
	return "Macro";
}

inline void Macro::TimeStamp(long time){
	if( !m_Commands.empty() ){
		const long deltaTime = time - m_Commands.front()->TimeStamp();

		for( auto pCommand : m_Commands )
			pCommand->TimeStamp( pCommand->TimeStamp() + deltaTime );
	}

	// Macro carries timestamp of its first entry
	Command::TimeStamp( time );
}

inline void Macro::SetAllTimeStamps( long time ){
	Command::SetAllTimeStamps( time );

	for( auto pCommand : m_Commands )
		pCommand->SetAllTimeStamps( time );
}

inline bool Macro::Valid() const noexcept{
	for( auto pCommand : m_Commands )
		if( !pCommand->Valid() )
			return false;

	return true;
}

inline bool Macro::Profound() const noexcept{
	for( auto pCommand : m_Commands )
		if( !pCommand->Profound() )
			return false;

	return true;
}

inline bool Macro::Reversible() const noexcept{
	for( auto pCommand : m_Commands )
		if( !pCommand->Reversible() )
			return false;

	return true;
}

inline std::unique_ptr<Command> Macro::Clone() const{
	return std::unique_ptr<Macro>( new Macro(*this) );
}

inline unsigned int Macro::EstimateExecutionTime() const noexcept{
	unsigned int result = 0;

	for( auto pCommand : m_Commands )
		result += pCommand->EstimateExecutionTime();

	return result;
}

inline void Macro::Log( Logbook& log ) const{
	for( const Command* pCommand : m_Commands )
		log( *pCommand );
}

inline void Macro::RegisterCommand( std::unique_ptr<Command> pCommand ){
	m_RegisteredCommads[pCommand->Name()] = std::move( pCommand );
}

inline std::unique_ptr<Command> Macro::CommandFrom( const std::string& name ) const{
	auto iter = m_RegisteredCommads.find( name );
	if( iter != m_RegisteredCommads.end() )
		return iter->second->Clone();

	return nullptr;
}

inline void Macro::_PopFront(){
	m_Commands.pop_front();
	if( !m_Commands.empty() )
		Command::TimeStamp( m_Commands.front()->TimeStamp() );
}
///////////////////////////////////////
inline void ThrowParserError( long timeStamp, const std::string& name, const std::string& message ){
	std::ostringstream stream;
	stream << "Parser Error: ";
	stream << '$' << timeStamp << ' ';
	stream << name << ": ";
	stream << message;
	throw std::runtime_error( stream.str() );
}

inline std::istream& ReadCommandFrom( std::istream& istream, Macro& macro )
{
	long timeStamp = -1;
	std::string name = "unknown";

	char prompt = 0;
	istream >> prompt;
	if( !istream ){
		if( istream.eof() )
			return istream;
		else
			ThrowParserError( timeStamp, name, "No time stamp!" );
	}

	if( prompt != '$' )// no more punchlines...
		return istream;

	istream >> timeStamp;
	if( !istream )
		ThrowParserError( timeStamp, name, "No time stamp!" );

	istream >> name;
	if( !istream )
		ThrowParserError( timeStamp, name, "No Command name!" );

	const auto n = name.find( '(' );
	if( n == std::string::npos ){
		char c;
		istream >> c;
		if( c != '(' ) ThrowParserError( timeStamp, name, "Missing opening parenthesis!" );
	}
	else{
		for( auto i = name.size() - 1; i > n ; --i )
			istream.putback( name[i] );

		name = name.substr( 0, n );
	}

	if( auto pCommand = macro.CommandFrom( name ) ){
		try{
			pCommand->ParameterFrom( istream );
		}
		catch( const std::exception& e ){
			ThrowParserError( timeStamp, name, e.what() );
		}

		pCommand->TimeStamp( timeStamp );
		macro.Push( std::move(pCommand) );
	}
	else
		ThrowParserError( timeStamp, name, "Unknown Command" );

	return istream;
}

inline std::ostream& operator<<( std::ostream& ostream, const Macro& macro ){
	OutStreamLog log( ostream );
	log( macro );
	return ostream;
}

inline std::istream& operator>>( std::istream& istream, Macro& macro ){
	while( !istream.eof() )
		ReadCommandFrom( istream, macro );

	return istream;
}
///////////////////////////////////////
inline History::History() noexcept{
	m_OnePastExecuted = m_CommandList.end();
}

inline History::History( const History& /*history*/ ) noexcept{
	assert( !"Not implemented yet!" );
}

inline History::History( History&& /*history*/ ) noexcept{
	assert( !"Not implemented yet!" );
}

inline History::~History() noexcept{
}

inline History& History::operator=( const History& /*history*/ ) noexcept{
	assert( !"Not implemented yet!" );
	return *this;
}

inline History& History::operator=( History&& /*history*/ ) noexcept{
	assert( !"Not implemented yet!" );
	return *this;
}

inline void History::Undo(){
	if( m_OnePastExecuted != m_CommandList.begin() )
		(*--m_OnePastExecuted)->Unexecute();
}

inline void History::Redo(){
	if( m_OnePastExecuted != m_CommandList.end() )
		(*m_OnePastExecuted++)->Execute();
}

inline bool History::GotUndoCommand() const{
	return m_OnePastExecuted != m_CommandList.begin();
}

inline bool History::GotRedoCommand() const{
	return m_OnePastExecuted != m_CommandList.end();
}

inline bool History::Empty() const noexcept{
	return m_CommandList.empty();
}

inline void History::Execute( std::unique_ptr<Command>& pCommand ){
	if( pCommand ){
		pCommand->Execute();
		Add( pCommand );
	}
}

inline void History::Add( std::unique_ptr<Command>& pCommand ){
	if( pCommand ){
		m_CommandList.erase( m_OnePastExecuted, m_CommandList.end() );
		m_CommandList.push_back( std::shared_ptr<Command>(pCommand.release()) );
		m_OnePastExecuted = m_CommandList.end();
	}
}

inline void History::Clear() noexcept{
	m_CommandList.clear();
	m_OnePastExecuted = m_CommandList.end();
}

inline std::unique_ptr<Macro> History::Macro() const noexcept{
	assert( !"Not implemented yet!" );
	return nullptr;
}
///////////////////////////////////////
inline void OutStreamLog::Log( const Command& command ){
	if( const Macro* pMacro = Macro::Cast( &command ) )
		pMacro->Log( *this );
	else
		m_Ostr << "$" << command.TimeStamp() << '\t' << command.Call() << std::endl;
}
///////////////////////////////////////
}