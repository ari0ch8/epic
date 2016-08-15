/*
** Haaf's Game Engine 1.5
** Copyright (C) 2003-2004, Relish Games
** hge.relishgames.com
**
** hgeParticleSystem helper class implementation
*/

#include "hgeparticle.h"
#include "hgeRandom.h"
#include <stdio.h>



using	namespace	Sexy;
using	namespace	HGE;

bool	hgeParticleSystem::m_bInitRandom = false;

hgeParticleSystem::hgeParticleSystem(const char *filename, DDImage *sprite, float fps /*= 0.0f*/, bool parseMetaData /*= true*/) // Change default behavior in header
{
	// For Software support
	/*	SWTri_AddAllDrawTriFuncs();//*/
	SWTri_AddDrawTriFunc(true, true, false, true, 0x8888, false, DrawTriangle_8888_TEX1_TALPHA1_MOD0_GLOB1_BLEND0);
	SWTri_AddDrawTriFunc(true, true, false, true, 0x888, false, DrawTriangle_0888_TEX1_TALPHA1_MOD0_GLOB1_BLEND0);
	SWTri_AddDrawTriFunc(true, true, false, true, 0x565, false, DrawTriangle_0565_TEX1_TALPHA1_MOD0_GLOB1_BLEND0);
	SWTri_AddDrawTriFunc(true, true, false, true, 0x555, false, DrawTriangle_0555_TEX1_TALPHA1_MOD0_GLOB1_BLEND0);//*/

	// LOAD DEFAULTS
	mbAdditiveBlend = false;
	mPlayMode = PLAY_ONCE;
	mAnimPlaying = false;
	mPlayMarker = 0;
	mPlayTime = 0.0f;
	mPlayTimer = 0.0f;
	mPlayTimerStepSize = 0.0f;

	mPingPong = PING;

	vecLocation.x=vecPrevLocation.x=0.0f;
	vecLocation.y=vecPrevLocation.y=0.0f;
	fTx=fTy=0;
	fEmissionResidue=0.0f;
	nParticlesAlive=0;
	fAge=-2.0;
	if(fps!=0.0f) fUpdSpeed=1.0f/fps;
	else fUpdSpeed=0.0f;
	fResidue=0.0f;

	rectBoundingBox.Clear();
	bUpdateBoundingBox=false;
	InitRandom();

	// LOAD FROM FILE
	PFILE *fp = p_fopen( filename, "rb" );
	if( fp == NULL )
		return ;
	p_fread( &info, sizeof( hgeParticleSystemInfo ), 1, fp );
	info.sprite = sprite; // info.sprite is bad

	if(parseMetaData) ParseMetaData(fp);

	p_fclose(fp);

}

hgeParticleSystem::hgeParticleSystem(hgeParticleSystemInfo *psi, float fps)
{
	memcpy(&info, psi, sizeof(hgeParticleSystemInfo));

	vecLocation.x=vecPrevLocation.x=0.0f;
	vecLocation.y=vecPrevLocation.y=0.0f;
	fTx=fTy=0;

	fEmissionResidue=0.0f;
	nParticlesAlive=0;
	fAge=-2.0;
	if(fps!=0.0f) fUpdSpeed=1.0f/fps;
	else fUpdSpeed=0.0f;
	fResidue=0.0f;

	rectBoundingBox.Clear();
	bUpdateBoundingBox=false;

	InitRandom();
	mbAdditiveBlend = false;
	mPlayMode = STOPPED;
	mAnimPlaying = false;
	mPlayMarker = 0;
}

hgeParticleSystem::hgeParticleSystem(const hgeParticleSystem &ps)
{
	memcpy(this, &ps, sizeof(hgeParticleSystem));

	InitRandom();
	//mbAdditiveBlend = false;
	mPlayMode = STOPPED;
	mAnimPlaying = false;
	mPlayMarker = 0;
}

void hgeParticleSystem::ParseMetaData(PFILE* aFile)
{
	BYTE	aMetaTag = -1; // If you Change Size of MetaTag, change it in SaveMetaData
	char aBuffer[MAX_PATH] = {0};
	memset(aBuffer, 0, MAX_PATH); //No Dirty Strings
	int	aSize = 0;

	// Read Returns Zero when it doesn't read something
	while(p_fread(&aMetaTag, sizeof(BYTE), 1, aFile))
	{
		switch(aMetaTag)
		{
		case ADDITIVE:
			{
				p_fread( &mbAdditiveBlend, sizeof(bool), 1, aFile );
				break; // Never forget to Break!
			}
		case POSITION:
			{
				if(p_fread( &vecPrevLocation, sizeof(vecPrevLocation), 1, aFile))
				{
					vecLocation.x=vecPrevLocation.x;
					vecLocation.y=vecPrevLocation.y;
				}
				break; // Never forget to Break!
			}
		case TEXTURE_PATH:
			{
				if(p_fread( &aSize, sizeof(int), 1, aFile) && p_fread(aBuffer, 1, aSize, aFile))
				{
					// Attemp to Load Texture
					mTextureName = StrFormat("%s", aBuffer);
					if(mTextureName != "")
					{
						DDImage* aTempImage = gSexyAppBase->GetImage(mTextureName);
						if(aTempImage != NULL)
						{
							info.sprite = aTempImage;
						}
					}
				}
				break; // Never forget to Break!
			}
		case POLYGON_POINTS:
			{
				if(p_fread( &aSize, sizeof(int), 1, aFile))
				{
					for(int i = 0; i < aSize; ++i)
					{
						Sexy::Point aPoint;
						if(p_fread(&aPoint, sizeof(Sexy::Point), 1, aFile))
							mPolygonClipPoints.push_back(aPoint);
					}
				}
				break; // Never forget to Break!
			}
		case WAY_POINTS:
			{
				if(p_fread( &aSize, sizeof(int), 1, aFile))
				{
					for(int i = 0; i < aSize; ++i)
					{
						Sexy::Point aPoint;
						if(p_fread(&aPoint, sizeof(Sexy::Point), 1, aFile))
							mWayPoints.push_back(aPoint);
					}
				}
				break; // Never forget to Break!
			}
		case ANIMATION_DATA:
			{
				p_fread(&mPlayTime, sizeof(mPlayTime), 1, aFile);
				p_fread(&mPlayMode, sizeof(mPlayMode), 1, aFile);
				break;
			}
			// TODO: Add your additional meta tags to this switch tree
		}
	}
}

void hgeParticleSystem::SaveFile(const char *filename)
{
	FILE* aFile = fopen(filename, "w+b");
	if(aFile != NULL)
	{
		// Standard Format
		fwrite(&(info), sizeof(hgeParticleSystemInfo), 1, aFile);

		// Extended Format
		SaveMetaData(aFile);

		fclose(aFile);
	}
}

void hgeParticleSystem::SaveMetaData(FILE* aFile)
{
	BYTE aMetaTag = 0;			// If you change size of MetaTag, besure to change it in ParseMetaData
	int aSize = 0;
	// Step 1: Write Meta Tag
	// Step 2: Write Size Info (Optional)
	// Step 3: Write Meta Data

	aMetaTag = ADDITIVE;
	fwrite(&aMetaTag, sizeof(aMetaTag), 1, aFile);
	fwrite(&mbAdditiveBlend, sizeof(bool), 1, aFile);

	aMetaTag = POSITION;
	fwrite(&aMetaTag, sizeof(aMetaTag), 1, aFile);
	fwrite(&vecLocation, sizeof(vecLocation), 1, aFile);

	aMetaTag = TEXTURE_PATH;
	fwrite(&aMetaTag, sizeof(aMetaTag), 1, aFile);
	aSize = (int)mTextureName.size();
	fwrite(&aSize, sizeof(int), 1, aFile);
	fwrite(mTextureName.c_str(),1, aSize, aFile);

	aMetaTag = POLYGON_POINTS;
	fwrite(&aMetaTag, sizeof(aMetaTag), 1, aFile);
	aSize = (int)mPolygonClipPoints.size();
	fwrite(&aSize, sizeof(int), 1, aFile);
	for(unsigned int i = 0; i < mPolygonClipPoints.size(); ++i)
	{
		fwrite(&mPolygonClipPoints[i], sizeof(Sexy::Point), 1, aFile);
	}

	aMetaTag = WAY_POINTS;
	fwrite(&aMetaTag, sizeof(aMetaTag), 1, aFile);
	aSize = (int)mWayPoints.size();
	fwrite(&aSize, sizeof(int), 1, aFile);
	for(unsigned int i = 0; i < mWayPoints.size(); ++i)
	{
		fwrite(&mWayPoints[i], sizeof(Sexy::Point), 1, aFile);
	}

	aMetaTag = ANIMATION_DATA;
	fwrite(&aMetaTag, sizeof(aMetaTag), 1, aFile);
	fwrite(&mPlayTime, sizeof(mPlayTime), 1, aFile);
	fwrite(&mPlayMode, sizeof(mPlayMode), 1, aFile);
}

void hgeParticleSystem::Update(float fDeltaTime)
{
   if(fUpdSpeed==0.0f) _update(fDeltaTime);
   else
   {
      fResidue+=fDeltaTime;
      if(fResidue>=fUpdSpeed)
	  {
			_update(fUpdSpeed);
			while(fResidue>=fUpdSpeed) fResidue-=fUpdSpeed;
	  }
   }
}

void hgeParticleSystem::_update(float fDeltaTime)
{
	int i;
	float ang;
	hgeParticle *par;
	hgeVector vecAccel, vecAccel2;

	if(fAge >= 0)
	{
		fAge += fDeltaTime;
		if(fAge >= info.fLifetime) fAge = -2.0f;
	}

	// Play Animation
	if(mAnimPlaying)
		_updatePlay(fDeltaTime);

	// update all alive particles

	if(bUpdateBoundingBox) rectBoundingBox.Clear();
	par=particles;

	for(i=0; i<nParticlesAlive; i++)
	{
		par->fAge += fDeltaTime;
		if(par->fAge >= par->fTerminalAge)
		{
			nParticlesAlive--;
			memcpy(par, &particles[nParticlesAlive], sizeof(hgeParticle));
			i--;
			continue;
		}

		vecAccel = par->vecLocation-vecLocation;
		vecAccel.Normalize();
		vecAccel2 = vecAccel;
		vecAccel *= par->fRadialAccel;

		// vecAccel2.Rotate(M_PI_2);
		// the following is faster
		ang = vecAccel2.x;
		vecAccel2.x = -vecAccel2.y;
		vecAccel2.y = ang;

		vecAccel2 *= par->fTangentialAccel;
		par->vecVelocity += (vecAccel+vecAccel2)*fDeltaTime;
		par->vecVelocity.y += par->fGravity*fDeltaTime;

		par->vecLocation += par->vecVelocity;

		par->fSpin += par->fSpinDelta*fDeltaTime;
		par->fSize += par->fSizeDelta*fDeltaTime;
		par->colColor += par->colColorDelta*fDeltaTime;	//-----use hgeColor

		if(bUpdateBoundingBox) rectBoundingBox.Encapsulate(par->vecLocation.x, par->vecLocation.y);

		par++;
	}

	// generate new particles

	if(fAge != -2.0f)
	{
		float fParticlesNeeded = info.nEmission*fDeltaTime + fEmissionResidue;
		int nParticlesCreated = (unsigned int)fParticlesNeeded;
		fEmissionResidue=fParticlesNeeded-nParticlesCreated;

		par=&particles[nParticlesAlive];

		for(i=0; i<nParticlesCreated; i++)
		{
			if(nParticlesAlive>=MAX_PARTICLES) break;

			par->fAge = 0.0f;

			//random
			par->fTerminalAge = Random_Float(info.fParticleLifeMin, info.fParticleLifeMax);

			par->vecLocation = vecPrevLocation+(vecLocation-vecPrevLocation)*Random_Float(0.0f, 1.0f);
			par->vecLocation.x += Random_Float(-2.0f, 2.0f);
			par->vecLocation.y += Random_Float(-2.0f, 2.0f);

			ang=info.fDirection-M_PI_2+Random_Float(0,info.fSpread)-info.fSpread/2.0f;
			if(info.bRelative) ang += (vecPrevLocation-vecLocation).Angle()+M_PI_2;
			par->vecVelocity.x = cosf(ang);
			par->vecVelocity.y = sinf(ang);
			par->vecVelocity *= Random_Float(info.fSpeedMin, info.fSpeedMax);

			par->fGravity = Random_Float(info.fGravityMin, info.fGravityMax);
			par->fRadialAccel = Random_Float(info.fRadialAccelMin, info.fRadialAccelMax);
			par->fTangentialAccel = Random_Float(info.fTangentialAccelMin, info.fTangentialAccelMax);

			par->fSize = Random_Float(info.fSizeStart, info.fSizeStart+(info.fSizeEnd-info.fSizeStart)*info.fSizeVar);
			par->fSizeDelta = (info.fSizeEnd-par->fSize) / par->fTerminalAge;

			par->fSpin = Random_Float(info.fSpinStart, info.fSpinStart+(info.fSpinEnd-info.fSpinStart)*info.fSpinVar);
			par->fSpinDelta = (info.fSpinEnd-par->fSpin) / par->fTerminalAge;
			
			////-----use hgeColor
			par->colColor.r = Random_Float(info.colColorStart.r, info.colColorStart.r+(info.colColorEnd.r-info.colColorStart.r)*info.fColorVar);
			par->colColor.g = Random_Float(info.colColorStart.g, info.colColorStart.g+(info.colColorEnd.g-info.colColorStart.g)*info.fColorVar);
			par->colColor.b = Random_Float(info.colColorStart.b, info.colColorStart.b+(info.colColorEnd.b-info.colColorStart.b)*info.fColorVar);
			par->colColor.a = Random_Float(info.colColorStart.a, info.colColorStart.a+(info.colColorEnd.a-info.colColorStart.a)*info.fAlphaVar);

			par->colColorDelta.r = (info.colColorEnd.r-par->colColor.r) / par->fTerminalAge;
			par->colColorDelta.g = (info.colColorEnd.g-par->colColor.g) / par->fTerminalAge;
			par->colColorDelta.b = (info.colColorEnd.b-par->colColor.b) / par->fTerminalAge;
			par->colColorDelta.a = (info.colColorEnd.a-par->colColor.a) / par->fTerminalAge;

			if(bUpdateBoundingBox) rectBoundingBox.Encapsulate(par->vecLocation.x, par->vecLocation.y);

			nParticlesAlive++;
			par++;
		}
	}

	vecPrevLocation=vecLocation;
}

void hgeParticleSystem::_updatePlay(float fDeltaTime)
{
	// Play WayPoints
	if(mAnimPlaying)
	{
		hgeVector anAnimPosVec;

		mPlayTimer += fDeltaTime;

		while(mPlayTimer > mPlayTimerStepSize) //Fast Forward!
		{
			mPlayTimer -= mPlayTimerStepSize;

			switch(mPlayMode)
			{
			case PLAY_ONCE:
				{
					if(++mPlayMarker >= mWayPoints.size()-1)
					{
						mAnimPlaying = false;
					}
					break;
				}
			case PLAY_LOOPED:
				{
					mPlayMarker = (mWayPoints.size() > 1)? (++mPlayMarker)%(mWayPoints.size()-1) : 0;
					break;
				}
			case PLAY_PINGPONGED:
				{
					switch(mPingPong)
					{
					case PING:
						{
							if(++mPlayMarker >= mWayPoints.size())
							{
								mPlayMarker = mWayPoints.size() - 1;
								mPingPong = PONG;
							}	
							break;
						}
					case PONG:
						{
							if(--mPlayMarker < 0)
							{
								mPlayMarker = 0;
								mPingPong = PING;
							}	
							break;
						}
					default:
						{
							mPingPong = PING;
							break;
						}
					}
					break;
				}
			default:
				{
					mAnimPlaying = false;
					break;
				}
			}
		}

		if(mAnimPlaying && mPlayMarker < mWayPoints.size() - 1)
		{
			anAnimPosVec = hgeVector(mWayPoints[mPlayMarker+1].mX - mWayPoints[mPlayMarker].mX,mWayPoints[mPlayMarker+1].mY - mWayPoints[mPlayMarker].mY);
			anAnimPosVec.Normalize();

			if(mPlayTimerStepSize != 0.0f)
				anAnimPosVec *= mPlayTimer/mPlayTimerStepSize;

			Point aPosPoint(mWayPoints[mPlayMarker].mX + anAnimPosVec.x , mWayPoints[mPlayMarker].mY + anAnimPosVec.y);

			MoveTo(aPosPoint.mX, aPosPoint.mY);
		}
	}
}

void hgeParticleSystem::MoveTo(float x, float y, bool bMoveParticles)
{
	int i;
	float dx,dy;
	
	if(bMoveParticles)
	{
		dx=x-vecLocation.x;
		dy=y-vecLocation.y;

		for(i=0;i<nParticlesAlive;i++)
		{
			particles[i].vecLocation.x += dx;
			particles[i].vecLocation.y += dy;
		}

		vecPrevLocation.x=vecPrevLocation.x + dx;
		vecPrevLocation.y=vecPrevLocation.y + dy;
	}
	else
	{
		if(fAge==-2.0) { vecPrevLocation.x=x; vecPrevLocation.y=y; }
		else { vecPrevLocation.x=vecLocation.x;	vecPrevLocation.y=vecLocation.y; }
	}

	vecLocation.x=x;
	vecLocation.y=y;
}

void hgeParticleSystem::FireAt(float x, float y)
{
	Stop();
	MoveTo(x,y);
	Fire();
}

void hgeParticleSystem::Play(int thePlayMode )
{
	if(thePlayMode != MAX_PLAYMODES) mPlayMode = thePlayMode;

	if(mWayPoints.size() >= 2)
	{
		FireAt(mWayPoints[0].mX, mWayPoints[0].mY);
		mAnimPlaying = true;
		mPlayMarker = 0;

		mPlayTimerStepSize = (mPlayTime/mWayPoints.size() > 0)? mPlayTime/mWayPoints.size() : 0.05f;
		mPlayTimer = 0.0f;
	}
	else 
		mAnimPlaying = false;
}

void hgeParticleSystem::Fire()
{
	if(info.fLifetime==-1.0f) fAge=-1.0f;
	else fAge=0.0f;
	fResidue=0.0;
}

void hgeParticleSystem::Stop(bool bKillParticles)
{
	fAge=-2.0f;
	if(bKillParticles) 
	{
		nParticlesAlive=0;
		rectBoundingBox.Clear();
	}
}

void hgeParticleSystem::Render( Graphics *g )
{
	// Check to make sure the texture is valid
	if(info.sprite == NULL) return;

	int	blendMode = g->GetDrawMode();

	if( mbAdditiveBlend && blendMode == Graphics::DRAWMODE_NORMAL )
		g->SetDrawMode( Graphics::DRAWMODE_ADDITIVE );
	if( !mbAdditiveBlend && blendMode == Graphics::DRAWMODE_ADDITIVE )
		g->SetDrawMode( Graphics::DRAWMODE_NORMAL );

	g->SetColorizeImages( true );
	int i;
	//DWORD col;
	hgeParticle *par=particles;

	//col=info.sprite->GetColor();
	Color col = g->GetColor();

	/*****************************************************/
	//	This section sets up the poly points by wrapping //
	//	the front around to the back.					 //
	/*****************************************************/
	bool front_pushed = false;
	// Clip to Polygon  3 points is a triangle
	if(mPolygonClipPoints.size() > 2)
	{
		mPolygonClipPoints.push_back(mPolygonClipPoints.front());
		front_pushed = true;
	}
	/*****************************************************/
	/*****************************************************/

	for(i=0; i<nParticlesAlive; i++, par++)
	{
		/*****************************************************/
		// Clip to Polygon: 3 points is a triangle plus the  //
		// front pushed to the back							 //
		/*****************************************************/
		if(mPolygonClipPoints.size() > 3)
			if(!wn_PnPoly(Point((int)(par->vecLocation.x+fTx), (int)(par->vecLocation.y+fTy))))
				continue;
		/*****************************************************/
		/*****************************************************/

		//info.sprite->SetColor(par->colColor.GetHWColor());
		//hgeColor col2( par->colColor.GetHWColor() ); 
		DWORD	col2 = par->colColor.GetHWColor();

		//g->SetColor( Color( col2.r * 255, col2.g * 255, col2.b * 255, col2.a * 255 ) );
		g->SetColor( Color( GETR( col2 ), GETG( col2 ), GETB( col2 ), GETA( col2 ) ) );
			
		//info.sprite->RenderEx(par->vecLocation.x+fTx, par->vecLocation.y+fTy, par->fSpin*particles[i].fAge, par->fSize);
		Transform	t;

		t.RotateRad( par->fSpin*particles[i].fAge );
		t.Scale( par->fSize, par->fSize );

		if( gSexyAppBase->Is3DAccelerated() )
		{
			g->DrawImageTransformF( info.sprite, t, par->vecLocation.x+fTx, par->vecLocation.y+fTy );
		}
		else
		{
			if(!mbAdditiveBlend) // Works fine
				g->DrawImageTransform( info.sprite, t, par->vecLocation.x+fTx, par->vecLocation.y+fTy );

			// Ok, Several problems here.
			//		1. Software Rendering using a complex transform requires SWTri_AddAllDrawTriFuncs()
			//		2. Software SWTri rendering functions don't support Additive.
			//
			// The main cause of this problem boils down to three variables: Additive drawing, Rotation and 
			// scaling.  Rotation AND scaling make the transform complex and will not render additive.
			// So the solution is to remove one of the variables to simplify the situation.
			//
			// The two solutions are: ignore the rotation and use standard funtions (removes the rotation
			// variable) OR cache the scaling thusly eliminating that variable.
			//
			// I choose the former because it doesn't make the particles look like a step function and
			// most particles are round and you can't tell they are rotating. (and it's easier)

			else
			g->DrawImage(	info.sprite, 
							(int)(par->vecLocation.x + fTx - (info.sprite->GetWidth()*par->fSize)/2.0f), //Centered
							(int)(par->vecLocation.y + fTy - (info.sprite->GetHeight()*par->fSize)/2.0f), //Centered
							(int)(info.sprite->GetWidth()*par->fSize), 
							(int)(info.sprite->GetHeight()*par->fSize));
		}
	}

	if(front_pushed)
		mPolygonClipPoints.pop_back();

	//info.sprite->SetColor(col);
	g->SetColor( col );
	g->SetColorizeImages( false );
	g->SetDrawMode( blendMode );
}

void	hgeParticleSystem::InitRandom()
{
	if( !m_bInitRandom )
	{
		Random_Seed( 0 );
		m_bInitRandom = true;
	}

}

// isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2 on the line
//            <0 for P2 right of the line
//    See: the January 2001 Algorithm "Area of 2D and 3D Triangles and Polygons"
inline int isLeft( Point P0, Point P1, Point P2 )
{
    return ( (P1.mX - P0.mX) * (P2.mY - P0.mY)
            - (P2.mX - P0.mX) * (P1.mY - P0.mY) );
}
//===================================================================

// wn_PnPoly(): winding number test for a point in a polygon
//      Input:   P = a point,
//               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
//				 Basically, the First vertex is duplicated after the last vertex
//      Return:  wn = the winding number (=0 only if P is outside V[])
bool hgeParticleSystem::wn_PnPoly( Sexy::Point theTestPoint)
{
	int    wn = 0;    // the winding number counter

	// loop through all edges of the polygon
	for (unsigned int i = 0; i < mPolygonClipPoints.size() - 1; i++) 
	{   // edge from mPolygonClipPoints[i] to mPolygonClipPoints[i+1]
		if (mPolygonClipPoints[i].mY <= theTestPoint.mY) 
		{         // start y <= theTestPoint.mY
			if (mPolygonClipPoints[i+1].mY > theTestPoint.mY)      // an upward crossing
				if (isLeft( mPolygonClipPoints[i], mPolygonClipPoints[i+1], theTestPoint) > 0)  // theTestPoint left of edge
					++wn;            // have a valid up intersect
		}
		else 
		{                       // start y > theTestPoint.mY (no test needed)
			if (mPolygonClipPoints[i+1].mY <= theTestPoint.mY)     // a downward crossing
				if (isLeft( mPolygonClipPoints[i], mPolygonClipPoints[i+1], theTestPoint) < 0)  // theTestPoint right of edge
					--wn;            // have a valid down intersect
		}
	}

	return (wn != 0);
}
//===================================================================

// cn_PnPoly(): crossing number test for a point in a polygon
//      Input:   P = a point,
//               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
//      Return:  0 = outside, 1 = inside
// This code is patterned after [Franklin, 2000]
bool hgeParticleSystem::cn_PnPoly( Point theTestPoint)
{
    int    cn = 0;    // the crossing number counter

    // loop through all edges of the polygon
    for (unsigned int i=0; i<mPolygonClipPoints.size() - 1; i++) {    // edge from mPolygonClipPoints[i] to mPolygonClipPoints[i+1]
       if (((mPolygonClipPoints[i].mY <= theTestPoint.mY) && (mPolygonClipPoints[i+1].mY > theTestPoint.mY))    // an upward crossing
        || ((mPolygonClipPoints[i].mY > theTestPoint.mY) && (mPolygonClipPoints[i+1].mY <= theTestPoint.mY))) { // a downward crossing
            // compute the actual edge-ray intersect x-coordinate
            float vt = (float)(theTestPoint.mY - mPolygonClipPoints[i].mY) / (mPolygonClipPoints[i+1].mY - mPolygonClipPoints[i].mY);
            if (theTestPoint.mX < mPolygonClipPoints[i].mX + vt * (mPolygonClipPoints[i+1].mX - mPolygonClipPoints[i].mX)) // theTestPoint.mX < intersect
                ++cn;   // a valid crossing of y=theTestPoint.mY right of theTestPoint.mX
        }
    }
    return (cn&1) != 0;    // 0 if even (out), and 1 if odd (in)

}