//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
// 
// 
// Copyright (c) 2025 Trend Redaktions- und Verlagsgesellschaft mbH
// Copyright (c) 2019 Marc-Michael Horstmann
//
// Permission is hereby granted to any person obtaining a copy of this software 
// and associated source code (the "Software"), to use, view, and study the 
// Software for personal or internal business purposes, subject to the following 
// conditions:
//
// 1. Redistribution, modification, sublicensing, or commercial use of the 
// Software is NOT permitted without prior written consent from the copyright 
// holder.
//
// 2. The Software is provided "AS IS", without warranty of any kind, express 
// or implied.
//
// 3. All copies of the Software must retain this license notice.
//
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#include "Fleet_Imp.h"


#include "trax/rigid/trains/Bogie.h"
#include "trax/rigid/trains/RollingStock.h"
#include "trax/rigid/trains/Train.h"


#include <iostream>

namespace trax{

std::unique_ptr<Fleet> Fleet::Make() noexcept
{
	try{
		return std::make_unique<Fleet_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

Fleet_Imp::Fleet_Imp() noexcept
	:  m_bTrainGenerationEnabled{ true }
{}

const char*	Fleet_Imp::TypeName() const{
	return "Fleet";
}

IDType Fleet_Imp::Add( std::shared_ptr<Train> pTrain ){
	if( pTrain ){
		if( Decorator()->IsMember( *pTrain ) )
			return Fleet_Base::Add( pTrain );

		if( IDType retval = Fleet_Base::Add( pTrain ); retval )
		{
			if( m_bTrainGenerationEnabled )
				m_PlugEnumerators.emplace_back( std::make_unique<FPlugEnumerator>( *this, *pTrain ) );

			RegisterBogies( *pTrain );
			return retval;
		}
	}

	return 0;
}

IDType Fleet_Imp::AddRelaxed( std::shared_ptr<Train> pTrain ){
	if( pTrain ){
		if( IsMember( *pTrain ) )
			return Fleet_Base::AddRelaxed( pTrain );

		if( IDType retval = Fleet_Base::AddRelaxed( pTrain ); retval )
		{
			if( m_bTrainGenerationEnabled )
				m_PlugEnumerators.emplace_back( std::make_unique<FPlugEnumerator>( *this, *pTrain ) );

			RegisterBogies( *pTrain );
			return retval;
		}
	}

	return 0;
}

bool Fleet_Imp::Remove( Train* pTrain, bool zeroIDs ){
	if( !pTrain )
		return false;

	for( auto iter = m_PlugEnumerators.begin(); iter != m_PlugEnumerators.end(); ++iter )
	{
		if( **iter == *pTrain )
		{
			m_PlugEnumerators.erase( iter );
			break;
		}
	}

	UnregisterBogies( *pTrain );

	return Fleet_Base::Remove( pTrain, zeroIDs );
}

void Fleet_Imp::Clear() noexcept
{
	m_Bogies.clear();
	m_PlugEnumerators.clear();
	m_TrainsSeparated.clear();
	Fleet_Base::Clear();
}
//
//std::shared_ptr<Train> Fleet_Imp::Get( const std::string& name ) const{
//	for( auto iter = m_Container.begin(); iter != m_Container.end(); ++iter )
//		if( GetBogieFrom( std::dynamic_pointer_cast<Bogie>((*iter).second), name ) )
//			return (*iter).second;
//
//	return nullptr;
//}


void Fleet_Imp::EnableTrainGeneration( bool bEnable ) noexcept
{
	m_bTrainGenerationEnabled = bEnable;
}

bool Fleet_Imp::IsTrainGenerationEnabled() const noexcept{
	return m_bTrainGenerationEnabled;
}

bool Fleet_Imp::Start( Scene& /*scene*/ )
{
	return true;
}

void Fleet_Imp::Update( Time dt )
{
	SeparateTrains();

	std::vector<std::tuple<Bogie*,RailRunner::EndType,spat::Sphere<Length>>> activeCouplings;
	for( auto bogie : m_Bogies )
	{
		bogie->Update( dt );

		if( bogie->IsActivated( RailRunner::EndType::north ) )
		{
			activeCouplings.push_back( std::make_tuple( bogie, RailRunner::EndType::north, bogie->GetCoupling( RailRunner::EndType::north ) ) );
		}

		if( bogie->IsActivated( RailRunner::EndType::south ) )
		{
			activeCouplings.push_back( std::make_tuple( bogie, RailRunner::EndType::south, bogie->GetCoupling( RailRunner::EndType::south ) ) );
		}
	}

	// Couple all pairs of overlapping couplings (sphere-sphere intersection)
    for( size_t i = 0; i < activeCouplings.size(); ++i )
	{
		const auto& [bogieA, endA, sphereA] = activeCouplings[i];
		for( size_t j = i + 1; j < activeCouplings.size(); ++j ) 
		{
			const auto& [bogieB, endB, sphereB] = activeCouplings[j];

			if( bogieA->GetCouplingProps( endA ).CouplingTypeIdx == 0 ||
				bogieB->GetCouplingProps( endB ).CouplingTypeIdx == 0 )
				throw std::logic_error{ "Terminate, because invalid coupling should never be active!" };

			if(	bogieA->GetCouplingProps( endA ).CouplingTypeIdx != bogieB->GetCouplingProps( endB ).CouplingTypeIdx )
				continue; // Only couple if coupling types match

			if( (sphereB.Center() - sphereA.Center()).Length() < sphereA.Radius() + sphereB.Radius() )
			{
				if( bogieA->Couple( endA, *bogieB, endB ) )
				{
					if( m_bTrainGenerationEnabled )
						ProduceCommonTrain( *bogieA, endA, *bogieB, endB );
				}
				else
					std::cerr << "Couldn't couple active bogies: " << bogieA->Reference( "name" ) << " ID: " << bogieA->ID()
					          << " and " << bogieB->Reference( "name" ) << " ID: " << bogieB->ID()  << std::endl;
			}
		}
	}
}

void Fleet_Imp::Pause() noexcept
{
}

void Fleet_Imp::Resume() noexcept
{
}

void Fleet_Imp::Stop() noexcept
{
	if( m_bTrainGenerationEnabled )
		SeparateTrains();
}

void Fleet_Imp::RegisterBogies( const Train& train ) noexcept
{
	for( int idx = 0; idx < train.GetNumberOfComponents(); ++idx )
	{
		if( auto pTrainComponent = std::dynamic_pointer_cast<TrainComponent>(train.GetComponent( idx )) )
		{
			if( auto pRollingStock = std::dynamic_pointer_cast<RollingStock>(pTrainComponent) )
			{
				for( int bindex = 0; bindex < pRollingStock->GetNumberOfBogies(); ++bindex )
				{
					Bogie* pBogie = &pRollingStock->GetBogie( bindex );
					if( std::find( m_Bogies.begin(), m_Bogies.end(), pBogie ) == m_Bogies.end() )
					{
						m_Bogies.push_back( pBogie );
					}
				}
			}
			else if( auto pTrain = std::dynamic_pointer_cast<Train>(pTrainComponent) )
				RegisterBogies( *pTrain );
		}
	}
}

void Fleet_Imp::UnregisterBogies( const Train& train ) noexcept
{
	for( int idx = 0; idx < train.GetNumberOfComponents(); ++idx )
	{
		if( auto pTrainComponent = std::dynamic_pointer_cast<TrainComponent>(train.GetComponent( idx )) )
		{
			if( auto pRollingStock = std::dynamic_pointer_cast<RollingStock>(pTrainComponent) )
			{
				for( int bindex = 0; bindex < pRollingStock->GetNumberOfBogies(); ++bindex )
				{
					Bogie* pBogie = &pRollingStock->GetBogie( bindex );
					if( auto iter = std::find( m_Bogies.begin(), m_Bogies.end(), pBogie ); iter != m_Bogies.end() )
					{
						m_Bogies.erase( iter );
					}
				}
			}
			else if( auto pTrain = std::dynamic_pointer_cast<Train>(pTrainComponent) )
				UnregisterBogies( *pTrain );
		}
	}
}

void Fleet_Imp::SeparateTrains()
{
	for( Train* pSeparated : m_TrainsSeparated )
	{
		if( std::shared_ptr<trax::Train> pNewTrain = pSeparated->Separate(); pNewTrain )
			pNewTrain->Reference ( "Name", "Train_" + std::to_string( Add( pNewTrain ) ) );
	}
	m_TrainsSeparated.clear();
}

void Fleet_Imp::ProduceCommonTrain( 
	const Bogie& bogieA, 
	RailRunner::EndType endA, 
	const Bogie& bogieB, 
	RailRunner::EndType endB )
{
	Orientation orientationA = (endA == RailRunner::EndType::south ? Orientation::Value::para : Orientation::Value::anti);
	Orientation orientationB = (endB == RailRunner::EndType::north ? Orientation::Value::para : Orientation::Value::anti);
	std::pair<Train*,Orientation> trainA = bogieA.GetRollingStock()->GetTopmostTrain();
	std::pair<Train*,Orientation> trainB = bogieB.GetRollingStock()->GetTopmostTrain();

	if( trainA.first != trainB.first )
	{
		if( trainA.first->GetNumberOfComponents() == 1 && trainB.first->GetNumberOfComponents() == 1 )
		{
			if( trainA.first->MaxThrust() > trainB.first->MaxThrust() )
			{
				trainA.first->Take( 
					Back( orientationA + trainA.second ), 
					*trainB.first,
					Front( orientationB + trainB.second ) );
				assert( trainA.first->IsValid() );
				Remove( trainB.first );
			}
			else
			{
				trainB.first->Take( 
					Front( orientationB + trainB.second ), 
					*trainA.first,
					Back( orientationA + trainA.second ) );
				assert( trainB.first->IsValid() );
				Remove( trainA.first );
			}
		}
		else if( trainA.first->GetNumberOfComponents() == 1 )
		{
			trainB.first->Take( 
				Front( orientationB + trainB.second ), 
				*trainA.first,
				Back( orientationA + trainA.second ) );
			assert( trainB.first->IsValid() );
			Remove( trainA.first );
		}
		else if( trainB.first->GetNumberOfComponents() == 1 )
		{
			trainA.first->Take( 
				Back( orientationA + trainA.second ), 
				*trainB.first,
				Front( orientationB + trainB.second ) );
			assert( trainA.first->IsValid() );
			Remove( trainB.first );
		}
		else if( std::shared_ptr<Train> pNewTrain = Train::Make(); pNewTrain )
		{
			pNewTrain->Append( RailRunner::EndType::south, trainA.first->ThisTrainComponent(), orientationA + trainA.second );
			pNewTrain->Append( RailRunner::EndType::south, trainB.first->ThisTrainComponent(), orientationB + trainB.second );
			pNewTrain->Reference( "Name", "Train_" + std::to_string( Add( pNewTrain ) ) );
		}
		else
		{
			std::cerr << "Fleet_Imp::ProduceCommonTrain: Failed to create new train!" << std::endl;
		}
	}
}

Fleet_Imp::FPlugEnumerator::FPlugEnumerator( Fleet_Imp& parent, Train& train )
	: PlugEnumerator  {}
	, m_Parent        { parent }
	, m_Train         { train }
	, m_PlugToSeparate{ *this, &FPlugEnumerator::PulseToSeparate }
{
	m_Train.JackOnSeparation().InsertAndAppend( &m_PlugToSeparate );
}

const trax::Plug& Fleet_Imp::FPlugEnumerator::_GetPlug( int idx ) const
{
	if( idx == 0 )
		return m_PlugToSeparate;
	else
		throw std::range_error( "FPlugEnumerator::_GetPlug: index out of range" );
}

void Fleet_Imp::FPlugEnumerator::PulseToSeparate() noexcept
{
	m_Parent.m_TrainsSeparated.push_back( &m_Train );
}
//std::shared_ptr<DepotCollection> Fleet_Imp::Attach( std::shared_ptr<DepotCollection> pDepotCollection ) noexcept{
//	auto retval = m_pDepotCollection;
//	if( auto pCollection = dynamic_cast<DepotCollection_Imp*>(m_pDepotCollection.get()) ){
//		pCollection->SetFleet( nullptr );
//		pCollection->SetBatch( nullptr );
//	}
//	m_pDepotCollection = pDepotCollection;
//	if( auto pCollection = dynamic_cast<DepotCollection_Imp*>(m_pDepotCollection.get()) ){
//		pCollection->SetFleet( Decorator() );
//		pCollection->SetBatch( m_pModule ? m_pModule->GetBatch() : nullptr );
//	}
//	return retval;
//}
//
//DepotCollection* Fleet_Imp::GetDepotCollection() const noexcept{
//	return m_pDepotCollection.get();
//}
///////////////////////////////////////
//void AddRecursive( Fleet& fleet, std::shared_ptr<RailRunner> pRailRunner ){
//	if( pRailRunner ){
//		fleet.Add( pRailRunner );
//
//		if( auto pTrain = std::dynamic_pointer_cast<Train>(pRailRunner) ){
//			for( int idx = 0; idx < pTrain->NumRollingStock(); ++idx ){
//				AddRecursive( fleet, pTrain->Get( idx ) );
//				fleet.PopActive();
//			}
//		}
//
//		else if( auto pBogie = std::dynamic_pointer_cast<Bogie>(pRailRunner) ){
//			for( int idx = 0; idx < pBogie->CntBogies(); ++idx ){
//				AddRecursive( fleet, pBogie->Get( idx ) );
//				fleet.PopActive();
//			}
//		}
//	}
//}
//
//void RemoveRecursiveFrom( Fleet& fleet, RailRunner& railRunner ){
//	fleet.Remove( &railRunner );
//
//	if( auto pTrain = dynamic_cast<Train*>(&railRunner) ){
//		for( int idx = 0; idx < pTrain->NumRollingStock(); ++idx )
//			RemoveRecursiveFrom( fleet, *pTrain->Get( idx ) );
//	}
//
//	else if( auto pBogie = dynamic_cast<Bogie*>(&railRunner) ){
//		for( int idx = 0; idx < pBogie->CntBogies(); ++idx )
//			RemoveRecursiveFrom( fleet, *pBogie->Get( idx ) );
//	}
//}
///////////////////////////////////////
}
