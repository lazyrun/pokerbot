
TEMPLATE = subdirs

SUBDIRS = HoldemFolder/HoldemFolder.pro \
   HoldemAgent/HoldemAgent.pro \
	HoldemExecutor770/770lib.pro \
	HoldemExecutorAcad/acadlib.pro \
	HoldemExecutorCake/cakelib.pro \
	HoldemExecutorParty/partylib.pro \
	HoldemExecutorRSP/rsplib.pro \
	HoldemExecutorTitan/titanlib.pro \
	HoldemInstall/HoldemInstall.pro \
	HoldemUninstall/HoldemUninstall.pro

CONFIG += ordered