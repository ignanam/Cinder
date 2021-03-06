/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#if ! defined( __LP64__ )

#include "cinder/gl/gl.h"
#include "cinder/qtime/QuickTimeUtils.h"
#include "cinder/qtime/QuickTime.h"

#if defined( CINDER_MSW )
#	include <CVPixelBuffer.h>
#	include <ImageCompression.h>
#	include <Movies.h>
#	include <QuickTimeComponents.h>
#endif

using namespace std;

namespace cinder { namespace qtime {

bool dictionarySetValue( CFMutableDictionaryRef dict, CFStringRef key, SInt32 value )
{
	bool         setNumber = false;
    CFNumberRef  number    = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);
	
    if( number != NULL ) {
		CFDictionarySetValue( dict, key, number );
		CFRelease( number );
		setNumber = true;
	}
	
    return setNumber;
}

bool dictionarySetPixelBufferPixelFormatType( bool alpha, CFMutableDictionaryRef dict )
{
	bool setPixelBufferOptions = false;
	setPixelBufferOptions = dictionarySetValue( dict, kCVPixelBufferPixelFormatTypeKey, ( alpha ) ? k32BGRAPixelFormat : k24RGBPixelFormat );
	return  setPixelBufferOptions;
}

bool dictionarySetPixelBufferSize( const unsigned int width, const unsigned int height, CFMutableDictionaryRef dict )
{
	bool setSize = false;

	setSize = dictionarySetValue( dict, kCVPixelBufferWidthKey, width );
	setSize = setSize && dictionarySetValue( dict, kCVPixelBufferHeightKey, height );
	
	return setSize;
}

bool dictionarySetPixelBufferBytesPerRowAlignment( CFMutableDictionaryRef dict )
{
	bool setAlignment = false;
	setAlignment = dictionarySetValue( dict, kCVPixelBufferBytesPerRowAlignmentKey, 1 );
	return setAlignment;
}

void dictionarySetPixelBufferOpenGLCompatibility( CFMutableDictionaryRef dict )
{
	CFDictionarySetValue(dict, kCVPixelBufferOpenGLCompatibilityKey, kCFBooleanTrue);
}

bool dictionarySetPixelBufferOptions( unsigned int width, unsigned int height, bool alpha, CFMutableDictionaryRef *pixelBufferOptions )
{
	bool setPixelBufferOptions = false;
    CFMutableDictionaryRef  pixelBufferDict = CFDictionaryCreateMutable( kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks );

    if ( pixelBufferDict != NULL ) {
		if ( dictionarySetPixelBufferPixelFormatType( alpha, pixelBufferDict ) ) {
			if ( dictionarySetPixelBufferSize( width, height, pixelBufferDict ) ) {
				if ( dictionarySetPixelBufferBytesPerRowAlignment( pixelBufferDict ) ) {
					dictionarySetPixelBufferOpenGLCompatibility( pixelBufferDict );
					*pixelBufferOptions = pixelBufferDict;
					setPixelBufferOptions = true;
				}
			}
		}
	}
	
	return setPixelBufferOptions;
}

CFMutableDictionaryRef initQTVisualContextOptions( int width, int height, bool alpha )
{
	CFMutableDictionaryRef  visualContextOptions = NULL;
    CFMutableDictionaryRef  pixelBufferOptions   = NULL;
												   
	bool  setPixelBufferOptions = dictionarySetPixelBufferOptions( width, height, alpha, &pixelBufferOptions );
	
	if( pixelBufferOptions != NULL ) {
		if( setPixelBufferOptions ) {
			visualContextOptions = CFDictionaryCreateMutable( kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks );
			if( visualContextOptions != NULL ) {
				CFDictionarySetValue( visualContextOptions, kQTVisualContextPixelBufferAttributesKey, pixelBufferOptions );
			}
		}
			
		CFRelease( pixelBufferOptions );
	}
	
	return  visualContextOptions;
}

::ItemCount openMovieBaseProperties( QTNewMoviePropertyElement movieProps[10] )
{
	ItemCount moviePropCount = 0;
	Boolean boolTrue = true;

	movieProps[moviePropCount].propClass = kQTPropertyClass_MovieInstantiation;
	movieProps[moviePropCount].propID = kQTMovieInstantiationPropertyID_DontAskUnresolvedDataRefs;
	movieProps[moviePropCount].propValueSize = sizeof(boolTrue);
	movieProps[moviePropCount].propValueAddress = &boolTrue;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;

	movieProps[moviePropCount].propClass = kQTPropertyClass_NewMovieProperty;
	movieProps[moviePropCount].propID = kQTNewMoviePropertyID_Active;
	movieProps[moviePropCount].propValueSize = sizeof(boolTrue);
	movieProps[moviePropCount].propValueAddress = &boolTrue;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;
	 
	movieProps[moviePropCount].propClass = kQTPropertyClass_NewMovieProperty;
	movieProps[moviePropCount].propID = kQTNewMoviePropertyID_DontInteractWithUser;
	movieProps[moviePropCount].propValueSize = sizeof(boolTrue);
	movieProps[moviePropCount].propValueAddress = &boolTrue;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;

#if 0
	movieProps[moviePropCount].propClass = kQTPropertyClass_Context;
	movieProps[moviePropCount].propID = kQTContextPropertyID_VisualContext;
	movieProps[moviePropCount].propValueSize = sizeof(visualContext);
	movieProps[moviePropCount].propValueAddress = &visualContext;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;
	 
	movieProps[moviePropCount].propClass = kQTPropertyClass_Context;
	movieProps[moviePropCount].propID = kQTContextPropertyID_AudioContext;
	movieProps[moviePropCount].propValueSize = sizeof(audioContext);
	movieProps[moviePropCount].propValueAddress = &audioContext;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;
#endif

	return moviePropCount;
}

::Movie openMovieFromUrl( const Url &url )
{
	::Movie result;
	::QTNewMoviePropertyElement movieProps[10];
	ItemCount moviePropCount = 0;

	moviePropCount = openMovieBaseProperties( movieProps );

	shared_ptr<const __CFString> urlStringCF = shared_ptr<const __CFString>( ::CFStringCreateWithCString( kCFAllocatorDefault, url.str().c_str(), kCFStringEncodingUTF8 ), ::CFRelease );
	CFURLRef urlCFBase = ::CFURLCreateWithString( kCFAllocatorDefault, urlStringCF.get(), NULL );
	shared_ptr<const __CFURL> urlCF = shared_ptr<const __CFURL>( urlCFBase, ::CFRelease );
	// Store the movie properties in the array
	movieProps[moviePropCount].propClass = kQTPropertyClass_DataLocation;
	movieProps[moviePropCount].propID = kQTDataLocationPropertyID_CFURL;
	movieProps[moviePropCount].propValueSize = sizeof(CFURLRef);
	movieProps[moviePropCount].propValueAddress = (void*)&urlCFBase;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;
	 
	OSStatus err;
	if( (err = ::NewMovieFromProperties( moviePropCount, movieProps, 0, NULL, &result ) ) != noErr ) {
		throw QuickTimeFileInvalidExc();
	}

	return result;
}

::Movie openMovieFromPath( const std::string &path )
{
	::Movie result;
	QTNewMoviePropertyElement movieProps[10];
	ItemCount moviePropCount = 0;

	moviePropCount = openMovieBaseProperties( movieProps );

	::CFStringRef basePathCF = ::CFStringCreateWithCString( kCFAllocatorDefault, path.c_str(), kCFStringEncodingUTF8 );
	shared_ptr<const __CFString> pathCF = shared_ptr<const __CFString>( basePathCF, ::CFRelease );
	// Store the movie properties in the array
	movieProps[moviePropCount].propClass = kQTPropertyClass_DataLocation;
	movieProps[moviePropCount].propID = kQTDataLocationPropertyID_CFStringNativePath;
	movieProps[moviePropCount].propValueSize = sizeof(CFStringRef);
	movieProps[moviePropCount].propValueAddress = (void*)&basePathCF;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;

	OSStatus err;
	if( (err = ::NewMovieFromProperties( moviePropCount, movieProps, 0, NULL, &result ) ) != noErr ) {
		throw QuickTimePathInvalidExc();
	}
		
	return result;
}

Handle createPointerReferenceHandle( void *data, Size dataSize )
{
	Handle dataRef = NULL;
	PointerDataRefRecord ptrDataRefRec;
	OSErr err;

	ptrDataRefRec.data = data;
	ptrDataRefRec.dataLength = dataSize;

	// create a data reference handle for our data
	err = ::PtrToHand( &ptrDataRefRec, &dataRef, sizeof(PointerDataRefRecord) );
	if( err != noErr )
		return 0;

	return dataRef;
}

OSStatus ptrDataRefAddFileNameExtension( ComponentInstance dataRefHandler, const string &fileName )
{
	OSStatus osErr = noErr;
	unsigned char myChar = 0;
	Handle fileNameHandle = NULL;

	// create a handle with our file name string
	// if we were passed an empty string, then we need to add this null string (a single 0 byte) to the handle
	if( fileName.empty() )
		osErr = ::PtrToHand( &myChar, &fileNameHandle, sizeof(myChar) );
	else {
		shared_ptr<char> tempStr( new char[fileName.size()+1], checked_array_deleter<char>() );
		memcpy( &tempStr.get()[1], &fileName[0], fileName.size() );
		tempStr.get()[0] = fileName.size();
		osErr = ::PtrToHand( &tempStr.get()[0], &fileNameHandle, tempStr.get()[0] + 1 );
		if( osErr != noErr ) goto bail;
	}
	if( osErr != noErr) goto bail;

	// set the data ref extension for the data ref handler
	osErr = ::DataHSetDataRefExtension( dataRefHandler, fileNameHandle, kDataRefExtensionFileName );

 bail:
    if( fileNameHandle )         // no longer need this
        ::DisposeHandle( fileNameHandle );

    return osErr;

}


OSStatus ptrDataRefAddMIMETypeExtension( ComponentInstance dataRefHandler, const std::string &mimeType )
{
    OSStatus osErr = noErr;
    Handle mimeTypeHndl = NULL;

    if( mimeType.empty() )
        return paramErr;

	shared_ptr<char> tempStr( new char[mimeType.size()+1], checked_array_deleter<char>() );
	memcpy( &tempStr.get()[1], &mimeType[0], mimeType.size() );
	tempStr.get()[0] = mimeType.size();
    osErr = ::PtrToHand( &tempStr.get()[0], &mimeTypeHndl, tempStr.get()[0] + 1 );
    if( osErr != noErr ) goto bail;

    // set the data ref extension for the data ref handler
    osErr = ::DataHSetDataRefExtension( dataRefHandler, mimeTypeHndl, kDataRefExtensionMIMEType );

 bail:
    if( mimeTypeHndl )         // no longer need this
        ::DisposeHandle( mimeTypeHndl );

    return osErr;
}

Handle createPointerDataRefWithExtensions( void *data, size_t dataSize, const string &fileName, const string &mimeType )
{
	OSStatus err = noErr;
	Handle dataRef = NULL;
	ComponentInstance dataRefHandler = NULL;

	// First create a data reference handle for our data
	dataRef = createPointerReferenceHandle( data, dataSize );
	if( ! dataRef )
		goto bail;

	//  Get a data handler for our data reference
	err = OpenADataHandler(
			dataRef,                    /* data reference */
			PointerDataHandlerSubType,  /* data ref. type */
			NULL,                       /* anchor data ref. */
			(OSType)0,                  /* anchor data ref. type */
			NULL,                       /* time base for data handler */
			kDataHCanRead,              /* flag for data handler usage */
			&dataRefHandler);           /* returns the data handler */
	if( err ) goto bail;

	// We can add the filename to the data ref to help
	// importer finding process. Find uses the extension.
	// If we add a filetype or mimetype we must add a
	// filename -- even if it is an empty string
	if( ( ! fileName.empty() ) || ( ! mimeType.empty() ) ) {
		err = ptrDataRefAddFileNameExtension( dataRefHandler, fileName );
		if( err ) goto bail;
	}

	// to add MIME type information, add a classic atom followed by
	// a Pascal string holding the MIME type
	if ( ! mimeType.empty() ) {
		err = ptrDataRefAddMIMETypeExtension( dataRefHandler, mimeType );
		if( err ) goto bail;
	}

	// dispose old data ref handle because it does not contain our new changes
	::DisposeHandle( dataRef );
	dataRef = NULL;

	// re-acquire data reference from the data handler to get the new changes
	err = ::DataHGetDataRef( dataRefHandler, &dataRef );
	if( err ) goto bail;

	::CloseComponent( dataRefHandler );

	return dataRef;
 bail:
    if( dataRefHandler ) {
		::CloseComponent(dataRefHandler);
    }

    if( dataRef ) {         // make sure and dispose the data reference handle once we are done with it
        ::DisposeHandle( dataRef );
    }

    return NULL;
}

// Refer to http://developer.apple.com/technotes/tn/tn1195.html Technical Note TN1195 "Tagging Handle and Pointer Data References in QuickTime"
::Movie openMovieFromMemory( const void *data, size_t dataSize, const string &fileNameHint, const string &mimeTypeHint )
{
	Handle dataRefHandle = createPointerDataRefWithExtensions( const_cast<void*>( data ), dataSize, fileNameHint, mimeTypeHint );

	::Movie result;
/*	QTNewMoviePropertyElement movieProps[10];
	ItemCount moviePropCount = 0;
	DataReferenceRecord dataRef = {0};
	
	moviePropCount = openMovieBaseProperties( movieProps );

	// Store the movie properties in the array
	movieProps[moviePropCount].propClass = kQTPropertyClass_DataLocation;
	movieProps[moviePropCount].propID = kQTDataLocationPropertyID_QTDataHandler;
	movieProps[moviePropCount].propValueSize = sizeof(aQTDataRef);
	movieProps[moviePropCount].propValueAddress = (void*)&dataRefHandle;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;

	OSErr err;
	if( (err = NewMovieFromProperties(moviePropCount, movieProps, 0, NULL, &result) ) != noErr ) {
		throw QuickTimeErrorLoadingExc();
	}*/

	short myResID = 0;
	OSErr err = NewMovieFromDataRef( &result, newMovieActive | newMovieAsyncOK, &myResID, dataRefHandle, 'ptr ' );
	if( err )
		throw QuickTimeErrorLoadingExc();
	
	return result;
}

static void CVPixelBufferDealloc( void *refcon )
{
	::CVBufferRelease( (CVPixelBufferRef)(refcon) );
}

Surface8u convertCVPixelBufferToSurface( CVPixelBufferRef pixelBufferRef )
{
	CVPixelBufferLockBaseAddress( pixelBufferRef, 0 );
	uint8_t *ptr = reinterpret_cast<uint8_t*>( CVPixelBufferGetBaseAddress( pixelBufferRef ) );
	int32_t rowBytes = CVPixelBufferGetBytesPerRow( pixelBufferRef );
	OSType type = CVPixelBufferGetPixelFormatType( pixelBufferRef );
	size_t width = CVPixelBufferGetWidth( pixelBufferRef );
	size_t height = CVPixelBufferGetHeight( pixelBufferRef );
	SurfaceChannelOrder sco;
	if( type == k24RGBPixelFormat )
		sco = SurfaceChannelOrder::RGB;
	else if( type == k32ARGBPixelFormat )
		sco = SurfaceChannelOrder::ARGB;
	else if( type == k24BGRPixelFormat )
		sco = SurfaceChannelOrder::BGR;
	else if( type == k32BGRAPixelFormat )
		sco = SurfaceChannelOrder::BGRA;
	Surface result( ptr, width, height, rowBytes, sco );
	result.setDeallocator( CVPixelBufferDealloc, pixelBufferRef );
	return result;
}

} } // namespace cinder::qtime

#endif // ! defined( __LP64__ )