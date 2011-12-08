/*
 * fix-ffIRCd by Lucent (c) 2011
 * Resolves various issues on the bouncer caused by the server.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <znc/Modules.h>

class CFixFFIRCd;

class CFixFFIRCd : public CModule
{
public:
	MODCONSTRUCTOR(CFixFFIRCd) {
	}

	virtual ~CFixFFIRCd() {}
	
	/* Fix per il problema causato dal part su Ping Timeout e Ghost
	   che causa la perdita della lista canali impostata da joinare */
	virtual EModRet OnUserPart(CString& sChannel, CString& sMessage) 
	{
		if(sMessage.Equals("Ghost"))
			return HALTCORE;
		if(sMessage.Equals("Ping Timeout"))
			return HALTCORE;
		if(sMessage.find("KILL") != CString::npos)
			return HALTCORE;
		if(sMessage.find("kill") != CString::npos)
			return HALTCORE;
		return CONTINUE;
	}
	
	/* Per evitare che ci facciano HELP a catena */
	virtual void OnModCommand( const CString& sCommand )
	{
		PutModule("Questo modulo non richiede configurazioni.");
	}
	
};

MODULEDEFS(CFixFFIRCd, "Risolve delle incompatibilità tra ZNC e il demone")
	
	