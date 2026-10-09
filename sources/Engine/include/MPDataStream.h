#ifndef MPDataStream_H
#define MPDataStream_H

#include "MPEffPrerequisites.h"
#include "MPSharedPtr.h"

/** General class wrapping incoming data.
@remarks
This class performs basically the same tasks as std::basic_istream, 
except that it does not have any formatting capabilities, and is
designed to be subclassed to receive data from multiple sources,
including libraries which have no compatiblity with the STL's
stream interfaces. As such, this is an abstraction of a set of 
wrapper classes which pretend to be standard stream classes but 
can actually be implemented quite differently. 
@par
	Usually, if a plugin or application supplies an ArchiveFactory, it should also supply a derived class of MPDataStream that can access that kind of
	Archive stream, unless the default one is good enough
@note
	Thread safety is not guaranteed here, for performance reasons. If you need asynchronous access to the stream
	you must organise the mutual exclusion yourself and avoid the race
*/
class MINDPOWER_API MPDataStream
{
public:
	/// constructor that creates an unnamed stream
	MPDataStream() : m_Size(0) {}
	/// constructor that creates a named stream
	MPDataStream(const String& name) : m_Name(name), m_Size(0) {}
	virtual ~MPDataStream() {}

	// stream operations
	template<typename T> MPDataStream& operator>>(T& val);
	/** Reads the requested number of bytes from the stream, stopping at eof.
	@param buf pointer to the buffer
	@param count number of bytes to read
	@returns number of bytes read
	*/
	virtual size_t read(void* buf, size_t count) = 0;
	/** Reads a line of data from the stream.
	@remarks
		The delimiter is not part of the returned data and is skipped, so it will not turn up on the next read either. The buffer does contain the terminator.
	@param buf pointer to the buffer
	@param maxCount maximum length of the line to read, excluding the terminator
	@param delim the delimiter
	@returns number of bytes read, excluding the terminator
	*/
	virtual size_t readLine(char* buf, size_t maxCount, const String& delim = "\n") = 0;

	/** Returns a String containing the next line of data, optionally 
	trimmed for whitespace. 
	@remarks
	This is a convenience method for text streams only, allowing you to 
	retrieve a String object containing the next line of data. The data
	is read up to the next newline character and the result trimmed if
	required.
	@param 
	trimAfter If true, the line is trimmed for whitespace (as in 
	String.trim(true,true))
	*/
	virtual String getLine( bool trimAfter = true );

	/** Returns the data of the whole stream as a string. */
	virtual String getAsString(void);

	/** Skips a line in the stream.
	@param delim line delimiter
	@returns number of bytes skipped
	*/
	virtual size_t skipLine(const String& delim = "\n") = 0;

	/** Skips the given number of bytes. A negative value skips backwards over that many bytes. */
	virtual void skip(long count) = 0;

	/** Moves the stream read pointer to the given byte position.
	*/
	virtual void seek( size_t pos ) = 0;

	/** Returns the current offset of the pointer from the start */
	virtual size_t tell(void) const = 0;

	/** Returns true if eof has been reached.*/
	virtual bool eof(void) const = 0;

	/** Closes the stream. */
	virtual void close(void) = 0;

	/** Returns the size of the stream data; may be 0 when unknown. */
	size_t size(void) const { return m_Size; }
	/// returns the name of the stream, if it has one
	const String& getName(void) const { return m_Name; }

protected:
	/// name used to identify the resource (e.g. the resource name) (optional)
	String m_Name;		
	/// size of the data in the stream (0 when the size cannot be determined)
	size_t m_Size;
#define MP_STREAM_TEMP_SIZE 128
	/// scratch buffer used by the format readers
	char m_TmpArea[MP_STREAM_TEMP_SIZE];
};

typedef MPSharedPtr<MPDataStream> MPDataStreamPtr;
typedef std::list<MPDataStreamPtr> MPDataStreamList;
typedef MPSharedPtr<MPDataStreamList> MPDataStreamListPtr;

/** Public derived class of DataStream handling in-memory stream data.*/
class MINDPOWER_API MPMemoryDataStream : public MPDataStream
{
public:

	/** Wraps an existing memory block as a stream.
	@param pMem pointer to the existing memory
	@param size size of the memory block, in bytes
	@param freeOnClose if true, the memory is freed when the stream is released.
	*/
	MPMemoryDataStream(void* pMem, size_t size, bool freeOnClose = false);
	/** Wraps an existing memory block as a named stream.
	@param name name of the stream
	@param pMem pointer to the existing memory
	@param size size of the memory block, in bytes
	@param freeOnClose if true, the memory is freed when the stream is released.
	*/
	MPMemoryDataStream(const String& name, void* pMem, size_t size, 
		bool freeOnClose = false);
	/** Creates a stream from another stream.
	@param sourceStream the other data stream
	@param freeOnClose if true, the memory is freed when the stream is released.
	*/
	MPMemoryDataStream(MPDataStream& sourceStream, bool freeOnClose = true);
	/** Creates a stream from another stream.
	@param sourceStream the other data stream (weak reference)
	@param freeOnClose if true, the memory is freed when the stream is released.
	*/
	MPMemoryDataStream(MPDataStreamPtr& sourceStream, bool freeOnClose = true);
	/** Creates a named stream from another stream.
	@param name name of the stream
	@param sourceStream the other data stream
	@param freeOnClose if true, the memory is freed when the stream is released.
	*/
	MPMemoryDataStream(const String& name, MPDataStream& sourceStream, 
		bool freeOnClose = true);
	/** Creates a named stream from another stream.
	@param name name of the stream
	@param sourceStream the other data stream (weak reference)
	@param freeOnClose if true, the memory is freed when the stream is released.
	*/
	MPMemoryDataStream(const String& name, const MPDataStreamPtr& sourceStream, 
		bool freeOnClose = true);
	/** Creates an empty stream.
	@param size size of the stream
	@param freeOnClose if true, the memory is freed when the stream is released.
	*/
	MPMemoryDataStream(size_t size, bool freeOnClose = true);
	/** Creates an empty named stream.
	@param name name of the stream
	@param size size of the stream
	@param freeOnClose if true, the memory is freed when the stream is released.
	*/
	MPMemoryDataStream(const String& name, size_t size, bool freeOnClose = true);

	~MPMemoryDataStream();

	/** Returns the pointer to the start of this stream's memory block. */
	uchar* getPtr(void) { return m_Data; }

	/** Returns the pointer this stream is currently pointing at. */
	uchar* getCurrentPtr(void) { return m_Pos; }

	/** Sets whether the memory is freed when the stream is closed. */
	void setFreeOnClose(bool free) { m_FreeOnClose = free; }

	/** @copydoc DataStream::read		*/
	size_t read(void* buf, size_t count);
	/** @copydoc DataStream::readLine	*/
	size_t readLine(char* buf, size_t maxCount, const String& delim = "\n");
	/** @copydoc DataStream::skipLine	*/
	size_t skipLine(const String& delim = "\n");
	/** @copydoc DataStream::skip		*/
	void skip(long count);
	/** @copydoc DataStream::seek		*/
	void seek( size_t pos );
	/** @copydoc DataStream::tell		*/
	size_t tell(void) const;
	/** @copydoc DataStream::eof		*/
	bool eof(void) const;
	/** @copydoc DataStream::close		*/
	void close(void);

protected:
	/// pointer to the start of the data region
	uchar* m_Data;
	/// pointer to the current position
	uchar* m_Pos;
	/// pointer to the end of the data region
	uchar* m_End;
	/// whether the data is deleted on close
	bool m_FreeOnClose;			
};

typedef MPSharedPtr<MPMemoryDataStream> MPMemoryDataStreamPtr;

/** Public derived class of DataStream handling std::basic_istream data.*/
class MINDPOWER_API MPFileStreamDataStream : public MPDataStream
{
public:
	/** Builds a stream from a standard stl stream
	@param s pointer to the stl stream
	@param freeOnClose if true, the memory is freed when the stream is released.
	*/
	MPFileStreamDataStream(std::ifstream* s, bool freeOnClose = true);
	/** Builds a named stream from a standard stl stream
	@param name name of the stream
	@param s pointer to the stl stream
	@param freeOnClose if true, the memory is freed when the stream is released.
	*/
	MPFileStreamDataStream(const String& name, std::ifstream* s, bool freeOnClose = true);
	/** Builds a named stream from a standard stl stream, and states its size
	@remarks
	This variant tells the class the size of the stream too, which 
	means this class does not need to seek to the end of the stream 
	to determine the size up-front. This can be beneficial if you have
	metadata about the contents of the stream already.
	@param name name of the stream
	@param s pointer to the stl stream
	@param size size of the stream, in bytes
	@param freeOnClose if true, the memory is freed when the stream is released.
	*/
	MPFileStreamDataStream(const String& name, std::ifstream* s, size_t size, bool freeOnClose = true);

	~MPFileStreamDataStream();

	/** @copydoc DataStream::read		*/
	size_t read(void* buf, size_t count);
	/** @copydoc DataStream::readLine	*/
	size_t readLine(char* buf, size_t maxCount, const String& delim = "\n");
	/** @copydoc DataStream::skipLine	*/
	size_t skipLine(const String& delim = "\n");
	/** @copydoc DataStream::skip		*/
	void skip(long count);
	/** @copydoc DataStream::seek		*/
	void seek( size_t pos );
	/** @copydoc DataStream::tell		*/
	size_t tell(void) const;
	/** @copydoc DataStream::eof		*/
	bool eof(void) const;
	/** @copydoc DataStream::close		*/
	void close(void);

protected:
	std::ifstream* m_pStream;
	bool m_FreeOnClose;			
};

/** DataStream的公用派生类,用于处理C风格的文件处理的数据.*/
class MINDPOWER_API MPFileHandleDataStream : public MPDataStream
{
public:
	/// build a stream from a C style file
	MPFileHandleDataStream(FILE* handle);
	/// build a named stream from a C style file
	MPFileHandleDataStream(const String& name, FILE* handle);
	~MPFileHandleDataStream();

	/** @copydoc DataStream::read		*/
	size_t read(void* buf, size_t count);
	/** @copydoc DataStream::readLine	*/
	size_t readLine(char* buf, size_t maxCount, const String& delim = "\n");
	/** @copydoc DataStream::skipLine	*/
	size_t skipLine(const String& delim = "\n");
	/** @copydoc DataStream::skip		*/
	void skip(long count);
	/** @copydoc DataStream::seek		*/
	void seek( size_t pos );
	/** @copydoc DataStream::tell		*/
	size_t tell(void) const;
	/** @copydoc DataStream::eof		*/
	bool eof(void) const;
	/** @copydoc DataStream::close		*/
	void close(void);

protected:
	FILE* m_FileHandle;
};


#endif