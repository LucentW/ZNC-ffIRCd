/*
 * FFAntiIdle by Lucent. Original version is antiidle.cpp included in ZNC's
 * extra modules.
 * Copyright (C) 2004-2011. Read AUTHORS file included with ZNC to track the
 * original authors.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include "Nick.h"
#include "User.h"
#include "Chan.h"

class CFFAntiIdle;

class CFFAntiIdleJob : public CTimer
{
public:
	CFFAntiIdleJob(CModule* pModule, unsigned int uInterval, unsigned int uCycles, const CString& sLabel, const CString& sDescription)
		: CTimer(pModule, uInterval, uCycles, sLabel, sDescription) {}
	virtual ~CFFAntiIdleJob() {}

protected:
	virtual void RunJob();
};

class CFFAntiIdle : public CModule
{
public:
	MODCONSTRUCTOR(CFFAntiIdle) {
		SetInterval(30);
	}

	virtual ~CFFAntiIdle() {}

	virtual bool OnLoad(const CString& sArgs, CString& sErrorMsg)
	{
		if(!sArgs.Trim_n().empty())
			SetInterval(sArgs.ToInt());
			
		return true;
	}

	virtual void OnModCommand( const CString& sCommand )
	{
		CString sCmdName = sCommand.Token(0).AsLower();
		if(sCmdName == "set")
		{
			CString sInterval = sCommand.Token(1, true);
			SetInterval(sInterval.ToInt());

			if(m_uiInterval == 0)
				PutModule("Anti-idle disattivato.");
			else
				PutModule("Anti-idle impostato a " + CString(m_uiInterval) + " secondi.");
		} else if(sCmdName == "off") {
			SetInterval(0);
			PutModule("Anti-idle disattivato.");
		} else if(sCmdName == "show") {
			if(m_uiInterval == 0)
				PutModule("L'anti-idle è disattivato.");
			else
				PutModule("Anti-idle impostato a " + CString(m_uiInterval) + " secondi.");
		} else {
			PutModule("Comandi: set, off, show");
		}
	}

	virtual EModRet OnChanMsg(CNick& Nick, CChan& Channel, CString& sMessage) 
	{
		if(Nick.GetNick() == GetUser()->GetIRCNick().GetNick()
				&& sMessage == "I-love ffIRCd")
			return HALTCORE;

		return CONTINUE;
	}

	virtual EModRet OnIRCConnected()
	{
		PutIRC("JOIN #devnull.ff");
		PutUser(":*ff_antiidle!znc@devnull.forumfree.it PRIVMSG #devnull.ff :[INFO] Per il corretto funzionamento del modulo antiidle, è sconsigliato uscire dal canale #devnull.ff.");
		PutUser(":*ff_antiidle!znc@devnull.forumfree.it PRIVMSG #devnull.ff :[INFO] E' possibile comunque effettuare il detach dal canale con il comando /znc Detach #devnull.ff");
		PutUser(":*ff_antiidle!znc@devnull.forumfree.it PRIVMSG #devnull.ff :[INFO] Questo messaggio non sarà mostrato fino alla prossima riconnessione.");
	}
	
private:
	void SetInterval(int i)
	{
		if(i < 0)
			return;

		m_uiInterval = i;

		RemTimer("FFAntiIdle");

		if(m_uiInterval == 0) {
			return;
		}

		AddTimer(new CFFAntiIdleJob(this, m_uiInterval, 0, "FFAntiIdle", "Periodicamente invia un messaggio all'utente stesso."));
	}

	unsigned int    m_uiInterval;
};

void CFFAntiIdleJob::RunJob() {
	GetModule()->PutIRC("PRIVMSG #devnull.ff :I-love ffIRCd");
}

MODULEDEFS(CFFAntiIdle, "Nasconde il tuo idle reale.")
