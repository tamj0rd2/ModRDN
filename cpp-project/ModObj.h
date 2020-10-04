/////////////////////////////////////////////////////////////////////
//	File	: Interface for MOD's
//	Desc.	:
//		17.Nov.00 (c) Relic Entertainment Inc.
//
#pragma once

/////////////////////////////////////////////////////////////////////
//	Forward Declarations

class SimEngineInterface;
class EntityFactory;
class TriggerFactory;
class ObjectiveFactory;
class BlipFactory;

class SelectionInterface;
class CameraInterface;
class SoundInterface;
class FXInterface;
class GhostInterface;
class DecalInterface;
class TerrainOverlayInterface;
class UIInterface;
class CharacterMap;

class RDNWorld;

class ModSimVis;

/////////////////////////////////////////////////////////////////////
//	ModObj, this class holds onto interfaces form the Engine

class ModObj
{
	// construction -- singleton
private:
	ModObj();
	~ModObj();

public:
	static ModObj *i();

	static void Initialize(SimEngineInterface *);
	static void Shutdown();

	// interface
public:
	RDNWorld *GetWorld();

	EntityFactory *GetEntityFactory();

	TriggerFactory *GetTriggerFactory();

	SoundInterface *GetSoundInterface();

	FXInterface *GetFxInterface();

	GhostInterface *GetGhostInterface();

	DecalInterface *GetDecalInterface();

	TerrainOverlayInterface *
	GetTerrainOverlayInterface();

	CharacterMap *GetCharacterMap();

	SelectionInterface *GetSelectionInterface();

	CameraInterface *GetCameraInterface();

	UIInterface *GetUIInterface();

	//
	void SetSelectionInterface(SelectionInterface *);

	void SetCameraInterface(CameraInterface *);

	void SetSoundInterface(SoundInterface *);

	void SetFxInterface(FXInterface *);

	void SetGhostInterface(GhostInterface *);

	void SetDecalInterface(DecalInterface *);

	void SetTerrainOverlayInterface(TerrainOverlayInterface *);

	void SetUIInterface(UIInterface *);

public:
	void CreateWorld(bool bMissionEd);

	ObjectiveFactory *GetObjectiveFactory();

	BlipFactory *GetBlipFactory();

	// fields
private:
	class Data;
	Data *m_pimpl;
};
