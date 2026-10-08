#ifndef MPParameterInterface_H
#define MPParameterInterface_H

#include "MPEffPrerequisites.h"

/// list of the available parameters
enum MPParameterType
{
	PT_BOOL,
	PT_REAL,
	PT_INT,
	PT_UNSIGNED_INT,
	PT_SHORT,
	PT_UNSIGNED_SHORT,
	PT_LONG,
	PT_UNSIGNED_LONG,
	PT_STRING,
	PT_VECTOR3,
	PT_MATRIX3,
	PT_MATRIX4,
	PT_QUATERNION,
	PT_COLOURVALUE
};

/// parameter definition, used by the reflection mechanism of MPParameterInterface
class MINDPOWER_API MPParameterDef
{
public:
	String name;
	String description;
	MPParameterType paramType;
	MPParameterDef(const String& newName, const String& newDescription, MPParameterType newType)
		: name(newName), description(newDescription), paramType(newType) {}
};
typedef std::vector<MPParameterDef> MPParameterList;

/** Abstract class through which a parameter object gets and sets its parameters.*/
class MINDPOWER_API MPParamCommand
{
public:
	virtual String doGet(const void* target) const = 0;
	virtual void doSet(void* target, const String& val) = 0;

	virtual ~MPParamCommand() { }
};
typedef std::map<String, MPParamCommand* > ParamCommandMap;

class MINDPOWER_API MPParamDictionary
{
	friend class MPParameterInterface;
protected:
	/// parameter definition
	MPParameterList m_ParamDefs;

	/// parameter object used for get/set
	ParamCommandMap m_ParamCommands;

	/** Returns a parameter object holding a named parameter. */
	MPParamCommand* getParamCommand(const String& name)
	{
		ParamCommandMap::iterator i = m_ParamCommands.find(name);
		if (i != m_ParamCommands.end())
		{
			return i->second;
		}
		else
		{
			return 0;
		}
	}

	const MPParamCommand* getParamCommand(const String& name) const
	{
		ParamCommandMap::const_iterator i = m_ParamCommands.find(name);
		if (i != m_ParamCommands.end())
		{
			return i->second;
		}
		else
		{
			return 0;
		}
	}
public:
	MPParamDictionary()  {}

	/** Adds a parameter definition for this class. 
	@param paramDef the MPParameterDef of the parameter
	@param paramCmd pointer to a derived parameter command object used to handle getting/setting.
	NB: this class does not destroy that pointer

	*/
	void addParameter(const MPParameterDef& paramDef, MPParamCommand* paramCmd)
	{
		m_ParamDefs.push_back(paramDef);
		m_ParamCommands[paramDef.name] = paramCmd;
	}

	const MPParameterList& getParameters(void) const
	{
		return m_ParamDefs;
	}
};

typedef std::map<String, MPParamDictionary> MPParamDictionaryMap;

/** Defines the common interface for setting custom parameters in a reflection-like style.
@remarks
	This class holds the class static name-to-parameter map dictionary used by subclasses. See MPParamDictionary for details. 
@remarks
	To use this class every subclass must call createParamDictionary in its constructor to build that class parameter dictionary.
*/
class MINDPOWER_API MPParameterInterface
{
public:

	virtual ~StringInterface() {}

	MPParamDictionary* getParamDictionary(void)
	{
		MPParamDictionaryMap::iterator i = m_sDictionary.find(m_ParamDictName);
		if (i != m_sDictionary.end())
		{
			return &(i->second);
		}
		else
		{
			return 0;
		}
	}

	const MPParamDictionary* getParamDictionary(void) const
	{
		MPParamDictionaryMap::const_iterator i = m_sDictionary.find(m_ParamDictName);
		if (i != m_sDictionary.end())
		{
			return &(i->second);
		}
		else
		{
			return 0;
		}
	}

	const MPParameterList& getParameters(void) const;

	virtual bool setParameter(const String& name, const String& value);
	
	virtual void setParameterList(const NameValuePairList& paramList);

	virtual String getParameter(const String& name) const
	{
		// obtain the dictionary
		const MPParamDictionary* dict = getParamDictionary();

		if (dict)
		{
			// look up a parameter object
			const MPParamCommand* cmd = dict->getParamCommand(name);

			if (cmd)
			{
				return cmd->doGet(this);
			}
		}

		// failure
		return "";
	}

	virtual void copyParametersTo(MPParameterInterface* dest) const
	{
		// obtain the dictionary
		const MPParamDictionary* dict = getParamDictionary();

		if (dict)
		{
			// iterate over our own parameters
			ParameterList::const_iterator i;

			for (i = dict->mParamDefs.begin(); 
				i != dict->mParamDefs.end(); ++i)
			{
				dest->setParameter(i->name, getParameter(i->name));
			}
		}


	}

	static void cleanupDictionary () ;

protected:
	/** Inner method that creates the parameter dictionary for this class.
	@remarks
		This method checks whether the parameter dictionary for this class exists and creates it if not. NB: the class name must be supplied (RTTI is not used)
	@param
	className the class name (used by the dictionary)
	@returns
		true if the dictionary was created, false if it already existed
	*/
	bool createParamDictionary(const String& className)
	{
		m_ParamDictName = className;
		if (m_sDictionary.find(className) == m_sDictionary.end())
		{
			m_sDictionary[className] = ParamDictionary();
			return true;
		}
		return false;

	}

protected:

	/// parameter dictionary
	static MPParamDictionaryMap m_sDictionary;
	/// class name used for the lookup (must be initialised by the subclass)
	String m_ParamDictName;

};

#endif