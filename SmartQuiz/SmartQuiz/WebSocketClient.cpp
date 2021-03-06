// WebSocketClient.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "WebSocketClient.h"
#include <uWS/uWS.h>
#include <agents.h>
#include <ppltasks.h>

/*
	Layout: 
	1) Take in StreamKey and MacAddr
	1) Call Server to get session
	2) Send connection request to websocket server
	3) 

*/
Concurrency::call<int> *call;
Concurrency::timer<int> *timer;
int StartQuiz();
int StartAttendance();
int StopAttendance();
int StopQuiz();
int NextQuestion();
void ErrorCheck(void* user);

const char* EventsStrings[] = { "Connection", "Message", "Disconnection", "Close" };
const char* ProfileStrings[] = { "rrq" };
const char* ActionStrings[] = { "start", "stop", "end", "next", "teacher", "classroom" };
const char* MessageStrings[] = { "profile", "type", "SessionID", "action", "RrqID", "qID", "wsID" };

const char* GetTextForEvent(int enumVal)
{
	return EventsStrings[enumVal];
}

const char* GetTextForMessage(int enumVal)
{
	return MessageStrings[enumVal];
}

const char* GetTextForProfile(int enumVal)
{
	return ProfileStrings[enumVal];
}

const char* GetTextForAction(int enumVal)
{
	return ActionStrings[enumVal];
}

const char* ATTENDANCE = "attendance";
const char* STOPATTENDANCE = "stopattendance";
int attendance_condition = 0;

int SESSION = 0;
int Q_ID = 0;
int RRQ_ID = -1;
std::string connection = "";

int startsocket(int session, CWnd * connect_label) {
	COutputLogger("arguments " + session);
	SESSION = session;

	if (session == -1)
	{
		MessageBoxA(0, "Something went wrong. Unable to get relevant session. Restart", "Notice", MB_ICONINFORMATION);
		 // only for debugging. Show Error in prod
	}

	uWS::Hub h;

	if (Quiz::startUp()) {
		COutputLogger("Initialization for quiz is complete");
	}
	else {
		COutputLogger("Initialization ERROR!");
		MessageBoxA(0, "Error Initializing receiver. Restart", "Notice", MB_ICONINFORMATION);
	}

	h.onConnection([](uWS::WebSocket<uWS::CLIENT> *ws, uWS::HttpRequest req) {
		// Get Session
		COutputLogger("Connected!");
		// construct string to send to web server
		json j;
		j[GetTextForMessage(Message::PROFILE)] = GetTextForProfile(Profile::RRQ);
		j[GetTextForMessage(Message::TYPE)] = GetTextForEvent(Events::CONNECTION);
		j[GetTextForMessage(Message::ACTION)] = GetTextForAction(Action::CLASSROOMCONNECTION);
		j[GetTextForMessage(Message::SESSIONID)] = SESSION;

		std::string server_conn = j.dump();
		ws->send(server_conn.c_str());
	});

	h.onMessage([&](uWS::WebSocket<uWS::CLIENT> *ws, char *message, size_t length, uWS::OpCode opCode)
	{
		using namespace std;
		COutputLogger("Message received from server!");
		COutputLogger(string(message).substr(0, length).c_str());

		if (Utilities::IsJson(std::string(message).substr(0, length)))
		{
			json j = json::parse(std::string(message).substr(0, length));

			if (j.find(GetTextForMessage(Message::PROFILE)) != j.end()) {
				auto profile = j[GetTextForMessage(Message::PROFILE)].get<string>();

				if (profile.compare(string(GetTextForProfile(Profile::RRQ))) == 0) {
					auto action = j[GetTextForMessage(Message::ACTION)].get<string>();

					if (action.compare(string(GetTextForEvent(Events::CONNECTION))) == 0) {
						// Classroom connection
						connect_label->SetWindowText(_T("Connected"));
					}
					else
					{
						RRQ_ID = stoi(j[GetTextForMessage(Message::RRQID)].get<string>());
						Q_ID = j[GetTextForMessage(Message::QID)].get<int>();

						if (action.compare(string(GetTextForAction(Action::START))) == 0) {
							StartQuiz();
						}
						else if (action.compare(string(GetTextForAction(Action::STOP))) == 0) {
							StopQuiz();
						}
						else if (action.compare(string(GetTextForAction(Action::NEXT))) == 0) {
							NextQuestion();
						}
						else if (action.compare(string(GetTextForAction(Action::END))) == 0) {
							StopQuiz();
							RRQ_ID = -1;
						}
					}
 				}
			}
		}
		else
		{
			COutputLogger("we had an issue reading the JSON?");
		}
	});

	h.onDisconnection([&](uWS::WebSocket<uWS::CLIENT> *ws, int code, char *message, size_t length) {
		COutputLogger("CLIENT CLOSE: ");
		connect_label->SetWindowText(_T("Disconnected"));
	});		

	connection = Rest::getServerUrl(session);
	if (strcmp(connection.c_str(), "Failure") == 0) {
		MessageBoxA(0, "An Error Occurred Finding Server. Please contact support", "Notice", MB_ICONINFORMATION);
	}
	else
	{
		h.connect(connection.c_str());
	}

	h.onError([](void *user) {
		ErrorCheck(user);
	});

	// callback function polls HID device for data
	std::function<void()> callback = [] {
		if (attendance_condition) {
			printf("Attendance poll: %d\n", attendance_condition);
			Quiz::quiz_poll(1);
		}
		else
			Quiz::quiz_poll(0);
	};

	call = new concurrency::call<int>(
		[callback](int)
	{
		callback();
	});

	// create a concurrent timer that polls the HID every time_ms ms
	// this is important since it leaves the main thread open to receive calls on WebSocket
	int time_ms = 500; // default poll time 500ms
	timer = new concurrency::timer<int>(time_ms, 0, call, true); 
	
	h.run();

	// The following code is run when the program shuts down

	COutputLogger("Closing program");
	timer->stop();
	delete timer; // delete timer object to free memory
	Quiz::session_end();
	return 0;
}

void COutputLogger(const char* txt)
{
	std::ofstream log("output.txt", std::ios_base::app | std::ios_base::out);
	log << txt << std::endl;
}


int StartQuiz() {
	if (RRQ_ID == -1)
	{
		// Need to get RRQ ID
		COutputLogger("Need to get RRQ_ID");
		RRQ_ID = Rest::getRRQ(SESSION);
	}
	COutputLogger("Time to start quiz");
	timer->start();
	if (!Quiz::quiz_start())
	{
		Quiz::quiz_stop();
		Quiz::quiz_start();
	}
	return 1;
}

int StartAttendance() {
	COutputLogger("Start attendance");
	timer->start();
	attendance_condition = 1;
	printf("Setting attendance condition = 1\n");
	if (!Quiz::quiz_start())
	{
		Quiz::quiz_stop();
		Quiz::quiz_start();
	}
	return 1;
}

int StopAttendance() {
	std::cout << "Stop attendance" << std::endl;
	attendance_condition = 0;
	printf("Setting attendance condition = 0\n");
	timer->stop();
	if (!Quiz::quiz_stop())
	{
		Quiz::quiz_start();
		Quiz::quiz_stop();
	}
	return 1;
}

int StopQuiz() {
	COutputLogger("Stop quiz");
	timer->pause();
	if (!Quiz::quiz_stop())
	{
		Quiz::quiz_start();
		Quiz::quiz_stop();
	}
	return 1;
}

int NextQuestion() {
	COutputLogger("Next Question");
	timer->start();
	if (!Quiz::quiz_start())
	{
		Quiz::quiz_stop();
		Quiz::quiz_start();
	}
	return 1;
}

void ErrorCheck(void* user) {
	int protocolErrorCount = 0;
	switch ((long)user) {
	case 1:
		std::cout << "Client emitted error on invalid URI" << std::endl;
		getchar();
		break;
	case 2:
		std::cout << "Client emitted error on resolve failure" << std::endl;
		getchar();
		break;
	case 3:
		std::cout << "Client emitted error on connection timeout (non-SSL)" << std::endl;
		getchar();
		break;
	case 5:
		std::cout << "Client emitted error on connection timeout (SSL)" << std::endl;
		getchar();
		break;
	case 6:
		std::cout << "Client emitted error on HTTP response without upgrade (non-SSL)" << std::endl;
		getchar();
		break;
	case 7:
		std::cout << "Client emitted error on HTTP response without upgrade (SSL)" << std::endl;
		getchar();
		break;
	case 10:
		std::cout << "Client emitted error on poll error" << std::endl;
		getchar();
		break;
	case 11:
		protocolErrorCount++;
		std::cout << "Client emitted error on invalid protocol" << std::endl;
		if (protocolErrorCount > 1) {
			std::cout << "FAILURE:  " << protocolErrorCount << " errors emitted for one connection!" << std::endl;
			getchar();
		}
		break;
	default:
		MessageBoxA(0, "Could not connect to server, retry", "Notice", MB_ICONINFORMATION);
		COutputLogger("FAILURE: could not connect to websocket server");
		getchar();
	}
}