// $Header: /nfs/slac/g/glast/ground/cvs/GlastPolicy/src/GlastMain.cxx,v 1.8 2007/04/16 21:41:09 burnett Exp $

// Include files
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/System.h"
// system stuff
#ifdef WIN32
# include <direct.h>
#else
# include <unistd.h>
#endif
#include <time.h>
#include <stdio.h>

//------------------------------------------------------------------------------
//
//  Package    : GlastPolicy
//
//  Description: generic Gaudi Main Program
//
//------------------------------------------------------------------------------
/**
Standard Glast-Gaudi main.
Specify the job options file location by:
1) specification, local or global, on command line.
2) content of env var JOBOPTIONS
3) src/jobOptions

*/
// declare function that may reduce the execution priority
void setPriority();
#ifdef WIN32
#define chdir _chdir
#endif

void current_time(std::ostream& out=std::cout)
{   
        static bool first=true;
        static time_t start;
        if(first){ first=false; ::time(&start);}
        time_t aclock;
        ::time( &aclock );   
        char tbuf[25]; ::strncpy(tbuf, asctime( localtime( &aclock ) ),24);
        tbuf[24]=0;
        out<<  "Current time: " << tbuf
            << " ( "<< ::difftime( aclock, start) <<" s elapsed)" << std::endl;
}


int main( int argn, char** argc) {
    
    std::string joboptions_file="src/jobOptions.txt"; // default
    
    const char* job = ::getenv("JOBOPTIONS"); // check for env var
    
    if( argn>1 ) { joboptions_file = argc[1];} // priority to command arg.
    else if( job ) { joboptions_file = job; }
    std::cerr << "Starting Glast-Gaudi job with job options file " 
        << joboptions_file << std::endl;
    current_time(std::cerr);
    const char* newdir = ::getenv("GLEAM_CHDIR");
    if( newdir !=0) {
        std::cerr << "Changing default directory to: " << newdir << std::endl;
        ::chdir(newdir);
    }
    
    
    // Create an instance of an application manager
    IInterface* iface = Gaudi::createApplicationMgr();
    
    SmartIF<IProperty>     propMgr ( IID_IProperty, iface );
    SmartIF<IAppMgrUI>     appMgr  ( IID_IAppMgrUI, iface );

    if( !appMgr.isValid() || !propMgr.isValid() ) {
        std::cout << "Fatal error while creating the ApplicationMgr " << std::endl;
        return 1;
    }

  
    // Set properties of algorithms and services
    propMgr->setProperty( "JobOptionsPath", joboptions_file );
        
    // Run the application manager and process events
#ifdef WIN32
    setPriority();
#endif
    StatusCode status = appMgr->run();

    // All done - exit with 0 if success.
    current_time(std::cerr);
    return (status.isFailure()? 1 : 0);
    
}
