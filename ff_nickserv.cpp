/*
 * Copyright (C) 2011 Lucent.
 * See the ZNC's AUTHORS file for details on the original module. 
 * This module is based on the original "nickserv" bundled with ZNC.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include "User.h"

class CFFNickServ : public CModule
{
public:
	MODCONSTRUCTOR(CFFNickServ)
	{
	}

	virtual ~CFFNickServ()
	{
	}

	virtual bool OnLoad(const CString& sArgs, CString& sMessage)
	{
		if (sArgs.empty())
			m_sPass = GetNV("Password");
		else
			m_sPass = sArgs;

		return true;
	}

	virtual void OnModCommand(const CString& sCommand)
	{
		CString sCmdName = sCommand.Token(0).AsLower();
		if (sCmdName == "set") {
			CString sPass = sCommand.Token(1, true);
			m_sPass = sPass;
			SetNV("Password", m_sPass);
			PutModule("Impostato l'autoidentify");
		} else if (sCmdName == "clear") {
			m_sPass = "";
			DelNV("Password");
		} else if (sCmdName == "credits" ) {
			PutModule("ff_nickserv -- 2010 Lucent");
			PutModule("Basato sul modulo nickserv contenuto nel pacchetto dei moduli ZNC");
			PutModule("---------------------------------------------------------------------------");
			PutModule("Ringraziamenti:");
			PutModule("  Bowser - il committente");
			PutModule("   Hannibal_Smith - la mente del progetto Forumfree IRCd");
			PutModule("    Niccolo` e la bella gente che ha sviluppato l'IRCd");
			PutModule("     I moderatori/staffer/FFmod su #chat.ff"); 
			PutModule("      Altra bella gente da #ffmagazine.ff");
			PutModule("       ... e tu che stai usando questo modulo :E");
		} else {
			PutModule("Comandi: set <password>, clear, credits");
		}
	}

	virtual void OnIRCConnected()
	{
		if (!m_sPass.empty()) {
			PutIRC("nickserv IDENTIFY " + m_sPass);
		}
	}


private:
	CString	m_sPass;
};

MODULEDEFS(CFFNickServ, "Ti autentica ai servizi dell'IRCd di Forumfree")
