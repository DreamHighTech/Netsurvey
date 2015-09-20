
#pragma once

#include "Scanner.h"

// Define functions out of class

TScanFunction ScanIntDoDummy;	
TInitFunction ScanIntInitDummy;
TInfoFunction ScanIntInfoDummy;

TScanFunction ScanIntDoPing;	
TInitFunction ScanIntInitPing;
TInfoFunction ScanIntInfoPing;

TScanFunction ScanIntDoTTL;	
TInfoFunction ScanIntInfoTTL;

TScanFunction ScanIntDoHostname;	
TInfoFunction ScanIntInfoHostname;

TInitFunction ScanIntInitNetBIOS;
TFinalizeFunction ScanIntFinalizeNetBIOS;
TOptionsFunction ScanIntSetupNetBIOS;

TScanFunction ScanIntDoNetBIOSComputerName;
TInfoFunction ScanIntInfoNetBIOSComputerName;

TScanFunction ScanIntDoNetBIOSGroupName;
TInfoFunction ScanIntInfoNetBIOSGroupName;

TScanFunction ScanIntDoNetBIOSUserName;
TInfoFunction ScanIntInfoNetBIOSUserName;

TScanFunction ScanIntDoNetBIOSMacAddress;
TInfoFunction ScanIntInfoNetBIOSMacAddress;

/* for tcp connect */
TScanFunction ScanIntDoHttpConnect;
TInfoFunction ScanIntInfoHttpConnect;

TScanFunction ScanIntDoFtpConnect;
TInfoFunction ScanIntInfoFtpConnect;

TScanFunction ScanIntDoPop2Connect;
TInfoFunction ScanIntInfoPop2Connect;

TScanFunction ScanIntDoPop3Connect;
TInfoFunction ScanIntInfoPop3Connect;

TScanFunction ScanIntDoSmtpConnect;
TInfoFunction ScanIntInfoSmtpConnect;

TScanFunction ScanIntDoImapConnect;
TInfoFunction ScanIntInfoImapConnect;

TScanFunction ScanIntDoFingerConnect;
TInfoFunction ScanIntInfoFingerConnect;

