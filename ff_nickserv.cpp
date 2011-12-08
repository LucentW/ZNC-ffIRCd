/*
 * Copyright (C) 2011 Lucent.
 * See the ZNC's AUTHORS file for details on the original module. 
 * This module is based on the original "nickserv" bundled with ZNC.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <znc/Modules.h>
#include <znc/User.h>

class CFFNickServ : public CModule
{
public:
	MODCONSTRUCTOR(CFFNickServ)
	{
		AddHelpCommand();
		AddCommand("set", static_cast<CModCommand::ModCmdFunc>(&CFFNickServ::SetCommand),
			"password", "Imposta la password per l'autoidentify");
		AddCommand("clear", static_cast<CModCommand::ModCmdFunc>(&CFFNickServ::ClearCommand),
			"", "Rimuove la password dalle configurazioni interne");
		AddCommand("clear", static_cast<CModCommand::ModCmdFunc>(&CFFNickServ::CreditsCommand),
			"", "Mostra i crediti agli sviluppatori e ringraziamenti vari");
	}

	virtual ~CFFNickServ() {}
	
	void SetCommand(const CString& sLine) {
		SetNV("Password", sLine.Token(1, true));
		PutModule("Password impostata.");
	}

	void ClearCommand(const CString& sLine) {
		DelNV("Password");
		PutModule("Password rimossa.");
	}
	
	void CreditsCommand(const CString& sLine) {
		PutModule("ff_nickserv -- 2011 Lucent");
		PutModule("Basato sul modulo nickserv contenuto nel pacchetto dei moduli ZNC");
		PutModule("---------------------------------------------------------------------------");
		PutModule("Ringraziamenti:");
		PutModule("  Bowser - il committente");
		PutModule("   Hannibal_Smith - la mente del progetto Forumfree IRCd");
		PutModule("    Niccolo` e la bella gente che ha sviluppato l'IRCd");
		PutModule("     I moderatori/staffer/FFmod su #chat.ff"); 
		PutModule("      Altra bella gente da #ffmagazine.ff");
		PutModule("       ... e tu che stai usando questo modulo :E");
	}
	
	virtual bool OnLoad(const CString& sArgs, CString& sMessage)
	{
		if (!sArgs.empty()) {
			SetNV("Password", sArgs);
			SetArgs("");
		}

		return true;
	}

	virtual void OnIRCConnected()
	{
		if (GetNV("Password")) {
			PutIRC("nickserv IDENTIFY " + GetNV("Password"));
		}
	}

};

NETWORKMODULEDEFS(CFFNickServ, "Ti autentica ai servizi dell'IRCd di Forumfree")
