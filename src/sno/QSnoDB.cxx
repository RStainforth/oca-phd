/////////////////////////////////////////////////////////////////////////
//QSnoDB                                                               //
//                                                                     //
//C++ interface to SNODB.                                              //
//Request flat Titles banks from SNODB with date, time, and data type  //
//validity.                                                            //
/////////////////////////////////////////////////////////////////////////
//*-- Author : Mark Boulay 12/98                                       



#include "QSnoDB.h"

ClassImp(QSnoDB)

QSnoDB::QSnoDB(const char *server):QClient(kSDBPort, server)
{
  //Default constructor
  quiet = kFALSE;
  serverOK = kFALSE;
}

QSnoDB::~QSnoDB()
{
  //Destructor
}

QBank *QSnoDB::GetBank( const char *name, Int_t number, Int_t date, Int_t time , Int_t dtype)
{
  //QSnoDBRequest
  //Request packet used for communicating with QSnoDB.

  //Retrieve this bank from the QSnoDBServer.
  QSnoDBRequest request(name, number, date, time, dtype );
  return GetBank( &request );
}

QBank * QSnoDB::GetBank( QSnoDBRequest *request )
{
  //Retrieve the bank described in request from the server.
  if ( OpenServer("xmit") != 0 ){ 
    Warning("GetBank","Attempting to start server.");
    gSystem->Exec("qsnodb > /tmp/qsnodb.log &");
    if ( OpenServer("xmit") != 0 ){
      Warning("GetBank","Giving up!");
      serverOK = kFALSE;
      return 0;
    }
  }
  serverOK = kTRUE;

  fSendMessage = new TMessage();
  fSendMessage->SetWhat(kMESS_OBJECT);
  fSendMessage->WriteObject( request );
  fSocket->Send( *fSendMessage );
  delete fSendMessage;
  char buf[1024];
 
  fSocket->Recv( fRecvMessage );
  if (fRecvMessage->What() == kMESS_OBJECT ) 
  {
      if ( !strcmp(fRecvMessage->GetClass()->GetName(),"QBank"))
        {
  	  //OK, we've received a bank, carry on.
        }
      else
        {
          //We've received an object of the wrong type.
	  printf("QSnoDB: received object of type %s from server\n",fRecvMessage->GetClass()->GetName());
          CloseServer();
	  delete fRecvMessage;
	  return 0;
	}
  }
  else
    {
      //problem, give up and return NULL.
      printf("QSnoDB: Did not receive a kMESS_OBJECT from server\n");
      CloseServer();
      delete fRecvMessage;
      return 0;
    }

  //At this point, fRecvMessage contains a QBank object.
  QBank *bank;
  if ( fRecvMessage->What() == kMESS_OBJECT )
    {
      bank = (QBank *)fRecvMessage->ReadObject( fRecvMessage->GetClass() );
      if (!quiet) printf("QSnoDB: Received bank '%s %i' from server\n",bank->GetBankName(),bank->GetBankNumber());
    }
  else
    {
      if (!quiet) printf("Error receiving bank from server.\n");
      bank = 0;
    }
  

  CloseServer();

  delete fRecvMessage;
  return bank;  
}









