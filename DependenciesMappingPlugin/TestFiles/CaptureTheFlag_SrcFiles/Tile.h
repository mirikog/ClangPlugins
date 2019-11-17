/* Test file for Dependencies Mapping Plugin -
 Tile.h */

#pragma once
#include "Surface.h"
#include "GamePiece.h"

class Tile
{
	GamePiece * gamePiece;
	Surface surface;
public:
	Tile()
	{
		gamePiece = nullptr;
		surface = Surface::REGULAR;
	}

	void setTileType(Surface s)					{ surface = s; }
	void setGamePiece(GamePiece * _gp)			{ gamePiece = _gp; }
	GamePiece * getGamePieceOnTile() const		{ return gamePiece; }
	Surface getSurfaceType() const				{ return surface; }
};
