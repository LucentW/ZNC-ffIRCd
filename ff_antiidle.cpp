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

#include <znc/Nick.h>
#include <znc/User.h>
#include <znc/Chan.h>

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
		
		CChan* pChan = GetUser()->FindChan("#devnull.ff");
		if (!pChan) {
			GetUser()->AddChan("#devnull.ff", false);
			pChan = GetUser()->FindChan("#devnull.ff");	
			if (pChan) {
				pChan->ResetJoinTries();
				pChan->Enable();
				pChan->DetachUser();
				pChan->SetIsOn(true);
			}
		}
		
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
		} else if(sCmdName == "hidechannel") {
			CUser* pUser = GetUser();
			CChan* pChan = pUser->FindChan("#devnull.ff");
			if (pChan->IsDetached()) {
				pChan->AttachUser();
				PutModule("Canale joinato con successo."); 
			} else {
				pChan->DetachUser();
				PutModule("Canale nascosto con successo.");
			}
		} else {
			PutModule("Comandi: set, off, show, hidechannel");
		}
	}
	
	virtual EModRet OnRaw(CString &sLine) {
		/* Se ci autoinviamo un messaggio mentre siamo away, questo
		 * causerà una risposta 301 dal server, che non va rimbalzata
		 * al client. */

		if (sLine.Token(1).Equals("301") && sLine.Token(3).Equals(m_pNetwork->GetIRCNick().GetNick())) {
			return HALT;
		}

		return CONTINUE;
	}

	virtual EModRet OnChanMsg(CNick& Nick, CChan& Channel, CString& sMessage) 
	{
		if(sMessage == "I-love ffIRCd") /* Potrebbero esserci nel canale altre persone con lo stesso modulo */
			return HALTCORE;

		return CONTINUE;
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
	GetModule()->PutIRC("PRIVMSG #devnull.ff :I-love ffIRCd"); /* lol, frase random */
}

MODULEDEFS(CFFAntiIdle, "Nasconde il tuo idle reale.")

