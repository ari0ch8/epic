#include "Force.h"

using namespace Sexy;

Force::Force(void)
{
	mMass = 0.0f;
	mMagnetism = 0.0f;
	mActive = true;
	mSource = SexyVector3(0,0,0);
	
}

Force::~Force(void)
{
}


void Force::UpdateFloater(Floater& theFloater, float theFrac)
{
//	float distance;

	if( !mActive ) return;

	SexyVector3 accelerationNext;
	SexyVector3 displacement;

	displacement.x = mSource.x - theFloater.mPosition.x;
	displacement.y = mSource.y - theFloater.mPosition.y;
	displacement.z = mSource.z - theFloater.mPosition.z;

//	distance = sqrt(
//		(displacement.x * displacement.x) +
//		(displacement.y * displacement.y) );

	accelerationNext.x = displacement.x * theFrac * mMass;
	accelerationNext.y = displacement.y * theFrac * mMass;
	accelerationNext.z = displacement.z * theFrac * mMass;

	theFloater.mAcceleration.x = (theFloater.mAcceleration.x + accelerationNext.x) / 2.0f;
	theFloater.mAcceleration.y = (theFloater.mAcceleration.y + accelerationNext.y) / 2.0f;
	theFloater.mAcceleration.z = (theFloater.mAcceleration.z + accelerationNext.z) / 2.0f;


	//if( mMagnetism )
	//{
	//	int i = 1;
	//}

	//if( displacement.x < 0 )
	//{
	//	theFloater.mVelocity.x -= (theFrac * mMagnetism);
	//}
	//else
	//{
	//	theFloater.mVelocity.x += (theFrac * mMagnetism);
	//}

	//if( displacement.y < 0 )
	//{
	//	theFloater.mVelocity.y -= (theFrac * mMagnetism);
	//}
	//else
	//{
	//	theFloater.mVelocity.y += (theFrac * mMagnetism);
	//}

	//theFloater.mPosition.x = mSource.x - displacement.x;
	//theFloater.mPosition.y = mSource.y - displacement.y;



	// theFloater.mAcceleration.Cross( acceleration );

	/*
	TODO: WTF?

	float angle;
	float cosAngle;
	float cosAngleSquared;
	float sinAngle;
	float sinAngleSquared;
	float distance;
	float distanceOffset;
	float distanceNext;
	float distanceNextSquared;

	theFloater.mPositionNext.x = theFloater.mPosition.x + (theFloater.mVelocity.x * theFrac);
	theFloater.mPositionNext.y = theFloater.mPosition.y + (theFloater.mVelocity.y * theFrac);

	distance = sqrt(
		((theFloater.mPositionNext.x - mSource.x) * (theFloater.mPositionNext.x - mSource.x)) +
		((theFloater.mPositionNext.y - mSource.y) * (theFloater.mPositionNext.y - mSource.y)));

	distanceOffset = theFrac / distance;

	distanceNext = distance - distanceOffset;
	distanceNextSquared = distanceNext * distanceNext;

	angle = atan2( 
		theFloater.mPositionNext.x - mSource.x,
		theFloater.mPositionNext.y - mSource.y);

	cosAngle = cos(angle);
	cosAngleSquared = cosAngle * cosAngle;
	sinAngle = sin(angle);
	sinAngleSquared = sinAngle * sinAngle;

	theFloater.mPositionNext.x = sinAngle * distanceNext;
	theFloater.mPositionNext.y = cosAngle * distanceNext;

	theFloater.mVelocity.x = theFloater.mPositionNext.x - theFloater.mPosition.x;
	theFloater.mVelocity.y = theFloater.mPositionNext.y - theFloater.mPosition.y;

	*/
}