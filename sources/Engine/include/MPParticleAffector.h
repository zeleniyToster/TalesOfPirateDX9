#ifndef MPParticleAffector_H
#define MPParticleAffector_H

#include "MindPowerAPI.h"

class MPParticleSystem;
class MPParticle;

/** Abstract class implementing the particle affector interface.
@remarks
Particle affectors modify particles during their lifetime inside a particle system. They are packaged into
types, such as a 'vector force' affector or a 'fader' affector; each
type modifies particles in its own way, using its own parameters.
@par
Because many affector types are possible, the engine does not fix the set of affectors that are available. It
provides a few built-in ones, and also allows plugins or applications to extend the affector set. This can
be done by subclassing ParticleAffector to extend the emission behaviour you need, and at the same time
subclassing ParticleAffectorFactory to create objects of your new affector. You register this factory by
calling addAffectorFactory on the ParticleSystemManager, after which the new affector can be created by name
from code or from a script file
@par
Particle emitters are extended in the same way. With that, a particle system becomes highly
extensible when a particle effect is created, since the affector types and their parameters can be combined
freely.
*/
class MINDPOWER_API MPParticleAffector
{
public:
	MPParticleAffector(MPParticleSystem* mParticleSystem): m_ParticleSystem(mParticleSystem) {}

	virtual ~MPParticleAffector();

	/** Lets the affector initialise every newly created particle
	@remarks
	This is where the affector gets the chance to initialise its effect on the particles. An affector may want to initialise some
	or all particles, depending on the affector itself
	@param
	pParticle pointer to the particle being initialised.
	*/
	virtual void _initParticle(MPParticle* pParticle) { /* by default do nothing */ }

	/** Allows the affector data to be applied to all active particles
	@remarks
	This is where the affector gets the chance to apply its data to the particles in the system. An affector
	may want to apply its effect to some or all particles, depending on the affector method
	@param
	pSystem pointer to the particle system being acted on.
	@param
	timeElapsed time elapsed since the previous call, in seconds.
	*/
	virtual void _affectParticles(MPParticleSystem* pSystem, float timeElapsed) = 0;

	/** Returns the affector type. 
	@remarks
	This parameter determines the affector type, so that the affector can be created from it
	*/
	const String &getType(void) const { return m_Type; }
protected:
	/** Inner method that sets up the base parameter definitions for subclasses
	@remarks
	Because StringInterface holds the parameter dictionary of every class, a subclass must call it to request
	that the base class add its parameters to the dictionary as well. Do not do this in the constructor, because the
	constructor is not run as a virtual function and has no virtual context
	@par
	A subclass must call its own createParamDictionary method before calling this method
	*/
	void addBaseParameters(void) { /* 实际上没有做任何事 - 将来可能会做一些事 */ }

protected:
	/// the particle affector type; must be initialised by the subclass
	string m_Type;

	MPParticleSystem* m_ParticleSystem;

};
#endif