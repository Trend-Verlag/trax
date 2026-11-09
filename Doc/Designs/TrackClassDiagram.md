# Track class diagram

This diagram includes the public Track interfaces and the implementation classes derived from them. The synchronized implementations inherit through `FrameSync_Imp` specializations, indicated on those inheritance edges.

```mermaid
classDiagram
	direction BT

	class Track {
		<<interface>>
	}
	class TrackBuilder {
		<<interface>>
	}
	class ParallelizableTrack {
		<<interface>>
	}
	class SectionTrack {
		<<interface>>
	}
	class MovableTrack {
		<<interface>>
	}
	class CollidableTrack {
		<<interface>>
	}
	class StaticTrack {
		<<interface>>
	}
	class MovableTrackAutoConnecting {
		<<interface>>
	}

	class Track_Imp
	class ParallelizableTrack_Imp
	class SectionTrack_Imp
	class MovableTrack_ImpBase
	class MovableTrack_Imp
	class CollidableTrack_ImpBase
	class CollidableTrack_Imp
	class StaticTrack_Imp
	class DynamicTrack_Imp
	class MovableTrackAutoConnecting_Imp

	Track <|-- TrackBuilder
	TrackBuilder <|-- ParallelizableTrack
	TrackBuilder <|-- SectionTrack : virtual
	TrackBuilder <|-- MovableTrack : virtual
	SectionTrack <|-- CollidableTrack : virtual
	SectionTrack <|-- StaticTrack : virtual
	MovableTrack <|-- MovableTrackAutoConnecting : virtual

	TrackBuilder <|-- Track_Imp : via TrackBase
	Track_Imp <|-- ParallelizableTrack_Imp
	ParallelizableTrack <|-- ParallelizableTrack_Imp
	Track_Imp <|-- SectionTrack_Imp : virtual
	SectionTrack <|-- SectionTrack_Imp : virtual
	Track_Imp <|-- MovableTrack_ImpBase : virtual
	MovableTrack <|-- MovableTrack_ImpBase : virtual
	MovableTrack_ImpBase <|-- MovableTrack_Imp : FrameSync_Imp specialization
	Track_Imp <|-- CollidableTrack_ImpBase : via SectionTrack_Imp
	SectionTrack_Imp <|-- CollidableTrack_ImpBase
	CollidableTrack <|-- CollidableTrack_ImpBase
	CollidableTrack_ImpBase <|-- CollidableTrack_Imp : FrameSync_Imp specialization
	SectionTrack_Imp <|-- StaticTrack_Imp
	StaticTrack <|-- StaticTrack_Imp
	MovableTrack_Imp <|-- DynamicTrack_Imp
	CollidableTrack_Imp <|-- DynamicTrack_Imp
	MovableTrackAutoConnecting <|-- MovableTrackAutoConnecting_Imp
	MovableTrack_Imp <|-- MovableTrackAutoConnecting_Imp
```

`TrackBase` is a type alias for the `TrackBuilder` implementation base, so `Track_Imp` derives from `TrackBuilder` through that alias. `TrackCollection` and `TrackSystem` hold tracks but are not Track subclasses.