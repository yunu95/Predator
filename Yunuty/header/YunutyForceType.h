#pragma once
namespace yunutyEngine
{
    namespace physics
    {
        enum class ForceType
        {
            FORCE,				//!< parameter has unit of mass * length / time^2, i.e., a force
            IMPULSE,			//!< parameter has unit of mass * length / time, i.e., force * time
            VELOCITY_CHANGE,	//!< parameter has unit of length / time, i.e., the effect is mass independent: a velocity change.
            ACCELERATION		//!< parameter has unit of length/ time^2, i.e., an acceleration. It gets treated just like a force except the mass is not divided out before integration.
        };
    }
}
