#ifndef MPFactoryObj_H
#define MPFactoryObj_H

/** Abstract factory class. It has no function of its own, but derived classes can add some.
*/
template< typename T > class FactoryObj
{
public:
	virtual ~FactoryObj() {};

	/** Returns the factory type.	*/
	virtual const string& getType() const = 0;

	/** Creates a new object.
	@param name name of the object to create
	@return the object created by this factory; its type depends on the factory.
	*/
	virtual T* createInstance( const string& name ) = 0;    
	/** Destroys an object created by this factory.
	@param ptr pointer to the object to destroy
	*/
	virtual void destroyInstance( T* ) = 0;    
};

#endif