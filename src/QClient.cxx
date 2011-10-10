/////////////////////////////////////////////////////////////////////////
//QClient                                                              //
//                                                                     //
//ABC implementing clients.                                            //
/////////////////////////////////////////////////////////////////////////
//*-- Author : Mark Boulay 20/1/98                                       

#include "QClient.h"
#include <string.h>

ClassImp(QClient)

QClient::QClient(Int_t port, const char *server)
{
  //Default constructor
  fSocket = 0;
  fSendMessage = 0;
  fRecvMessage = 0;
  fServer = server;
  fPort   = port;
}

QClient::~QClient()
{
  //Destructor
  delete fSocket;
}

Int_t QClient::OpenServer(const char *option)
{
  //Open a connection to the QServer.
  if ( fSocket )  { CloseServer(); }
  fSocket = new TSocket(fServer,fPort);
  if ( fSocket->GetErrorCode() ) 
    {
    Error("OpenServer","Server %s is not responding\n",(const char *)fServer );
    CloseServer();
    return -1;
    }
  char buf[256];
  fSocket->Recv(buf,256);
  if ( !strcmp( buf, "Connection established" ) )
    {
      //Connection is OK.  Let the server know our intentions.
      strcpy(buf,option);
      fSocket->Send(buf);
      return 0;
    }
  else { Error("OpenServer","Error opening connection to %s\n",(const char *)fServer); CloseServer(); return -1; }
}

Int_t QClient::CloseServer()
{
  //Close a connection to the QServer.

  fSocket->Close();
  delete fSocket; fSocket = 0;
  return 0;
}

Int_t QClient::PingServer()
{
  //Check if the server is alive.
  if ( OpenServer("ping") == 0 ) { CloseServer(); return 0; }
  else return -1;
}










