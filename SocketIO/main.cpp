#include <string>
#include <process.h>
#include <fstream>
#include "base64_helper.h"
#include "SocketServer.h"
#include "SocketClient.h"
#include "../json/json.h"

using namespace std;

#define PPCOUNT_PORT		3001			// 사람군집 솔루션 통신 포트
#define ABNORMAL_PORT		3002			// 이상감지 솔루션 통신 포트
#define SERVER_IP			"127.0.0.1"		// 영상분석서버 IP주소 샘플
//#define SERVER_IP			"119.204.158.157"		// 영상분석서버 IP주소 샘플
#define SHOP_ID				101				// 지점 아이디 샘플
#define DELAY				1000			// 통신 주기 샘플
#define SAMPLE_IMAGE_PATH	"../sample_image/abnormal.jpg"
#define SAVE_IMAGE_PATH	"../save_image/"
//#define TEST_MODE			

string makePeopleCountJsonData(int id);
string makeAbnormalJsonData(int id, string image_path);
string getSysTime();
bool AbnormalJsonData(string jsonFormat, bool verbose);

static DWORD WINAPI startClient(LPVOID parameter);
static DWORD WINAPI startServerMakeJson(LPVOID parameter);



int target_port;
int main()
{

	// 샘플 검출 모드
	enum mode{PPCOUNT, ABNORMAL};

	// 사람군집 솔루션으로 샘플 타겟 설정 
	//int mode = PPCOUNT;
	// 이상감지 솔루션으로 샘플 타겟 설정 
	int mode = ABNORMAL;

	switch (mode)
	{
	case PPCOUNT:
		target_port = 3001;
		break;
	case ABNORMAL:
		target_port = 3002;
		break;
	default:
		break;
	}
#ifdef TEST_MODE
	while (true)
	{
		SocketClient client;
		if (client.connect(SERVER_IP, target_port) == 0)
		{
			while (true)
			{
				string msg = client.receive();
				printf("%s\n", msg.c_str());

			}
		}

		client.disconnect();
	}
#else
	SocketServer server;
	// 샘플 클라이언트 생성 (키오스크)
	_beginthreadex(0, 0, (unsigned int(__stdcall *)(void *))startClient, 0, 0, NULL);
	// 샘플 서버 데이터 생성
	_beginthreadex(0, 0, (unsigned int(__stdcall *)(void *))startServerMakeJson, &server, 0, NULL);

	
	// 가상 서버 시작 (영상분석)
	if (server.init(target_port) != 0)
		return -1;
	if (server.start() != 0)
		return -1;
#endif

}


// 영상분석 샘플 데이터 생성 쓰레드
static DWORD WINAPI startServerMakeJson(LPVOID parameter)
{
	SocketServer* pSever = (SocketServer*)parameter;
	Sleep(2000);
	while (true)
	{	if(target_port == PPCOUNT_PORT)
			pSever->pushMessage(makePeopleCountJsonData(SHOP_ID).c_str()); 
		else if (target_port == ABNORMAL_PORT)
			pSever->pushMessage(makeAbnormalJsonData(SHOP_ID, string(SAMPLE_IMAGE_PATH)).c_str());
		Sleep(DELAY);
	}
	return 0;
}


// 솔루션에 접속하는 샘플 클라이언트 쓰레드
static DWORD WINAPI startClient(LPVOID parameter)
{
	SocketServer* pSever = (SocketServer*)parameter;
	printf("START CLIENT\n");
	while (true)
	{
		SocketClient client;
		if (client.connect(SERVER_IP, target_port) == 0)
		{
			while (true)
			{
				string msg = client.receive();
				printf("client_received:%d\n", msg.size());
				if (target_port == ABNORMAL_PORT)
					AbnormalJsonData(msg, true);

			}
		}
		
		client.disconnect();
	}
	return 0;
}


bool AbnormalJsonData(string jsonFormat, bool verbose)
{
	Json::Value jsonData;
	Json::Reader jsonReader;

	if (jsonReader.parse(jsonFormat.c_str(), jsonData))
	{
		const Json::Value objects = jsonData;
		printf("%s\n",objects["cam_idx"].asString().c_str());
		printf("%s\n", objects["time"].asString().c_str());
		printf("%s\n", objects["error"].asString().c_str());
		

		if (objects["abnormal"].asString().compare("true") == 0)
		{
			printf("%s\n", objects["image_file_name"].asString().c_str());
			//objects["image_data_base64"] = base64_code;

			string output = base64_decode(objects["image_data_base64"].asString());
			string save_file_path = SAVE_IMAGE_PATH + objects["image_file_name"].asString();

			std::ofstream out(save_file_path.c_str(), std::ios::binary | std::ios::out);
			if (out.is_open())
			{
				out << output;
				out.close();
			}

			for (auto url : objects["rtsp_address"])
				printf("%s\n", url.asString().c_str());

			/*
			const Json::Value objects = jsonData["config"];
			this->values.debug_mode_on = objects["debug_mode_on"].asBool();
			this->values.draw_mode_on = objects["draw_mode_on"].asBool();
			this->values.robot_id = objects["robot_id"].asInt();
			this->values.transmission_millisec = objects["transmission_millisec"].asInt();
			this->values.target_camera_rtsp = objects["target_camera_rtsp"].asString();
			this->values.object_min_size = objects["object_min_size"].asFloat();
			this->values.object_max_size = objects["object_max_size"].asFloat();
			this->values.tracker_assign_size = objects["tracker_assign_size"].asFloat();
			this->values.save_original_video_on = objects["save_original_video_on"].asBool();
			this->values.save_original_video_fps = objects["save_original_video_fps"].asFloat();
			*/
		}
		
	}
	return true;
}


string makePeopleCountJsonData(int id)
{
	string json;
	//Json::Value root;
	Json::Value evtInfo;
	Json::Value objVal;
	int person_count = 2;
	
	evtInfo["id"] = id;
	evtInfo["time"] = getSysTime().c_str();
	evtInfo["objects"] = Json::arrayValue;
	if (person_count > 0)
	{
		for (int i = 0; i<person_count; i++)
		{
			Json::Value val;
			Json::Value valRect;
			valRect["x"] = 10;
			valRect["y"] = 10;
			valRect["width"] = 100;
			valRect["height"] = 300;
			val["rect"] = valRect;
			objVal.append(val);
		}
		evtInfo["objects"] = objVal;
	}
	
	evtInfo["view_pos"] = "left";
	evtInfo["error"] = "";
	Json::StreamWriterBuilder wbuilder;
	wbuilder["indentation"] = "\t";
	json = Json::writeString(wbuilder, evtInfo);
	return json;
}



string makeAbnormalJsonData(int id, string img_path)
{
	string json;
	//Json::Value root;
	Json::Value evtInfo;
	Json::Value rtspVal;
	vector<string> rtsp_url;
	
	evtInfo["id"] = id;
	evtInfo["cam_idx"] = "both";
	evtInfo["time"] = getSysTime().c_str();
	evtInfo["abnormal"] = "true";
	evtInfo["error"] = "";
	evtInfo["rtsp_address"] = Json::arrayValue;

	if (evtInfo["abnormal"].compare("true") == 0)
	{
		if (evtInfo["cam_idx"].compare("left") == 0)
		{
			rtsp_url.push_back("rtsp://admin:admin@112.168.248.254:1001/cam/realmonitor?channel=1&subtype=1");
		}
		else if (evtInfo["cam_idx"].compare("right") == 0)
		{
			rtsp_url.push_back("rtsp://admin:admin@112.168.248.254:1002/cam/realmonitor?channel=1&subtype=1");
		}
		else if (evtInfo["cam_idx"].compare("both") == 0)
		{
			rtsp_url.push_back("rtsp://admin:admin@112.168.248.254:1001/cam/realmonitor?channel=1&subtype=1");
			rtsp_url.push_back("rtsp://admin:admin@112.168.248.254:1002/cam/realmonitor?channel=1&subtype=1");
		}

		for (string url : rtsp_url)
			evtInfo["rtsp_address"].append(url.c_str());

		std::ifstream infile(img_path, std::ifstream::binary);
		std::vector<char> data((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
		std::string string_data(data.begin(), data.end());
		std::string base64_code = base64_encode(string_data);
		evtInfo["image_file_name"] = img_path.substr(img_path.rfind('/')+1, img_path.size());
		evtInfo["image_data_base64"] = base64_code;
	}

	

	

	Json::StreamWriterBuilder wbuilder;
	wbuilder["indentation"] = "\t";
	json = Json::writeString(wbuilder, evtInfo);
	return json;
}

string getSysTime()
{
	time_t timer = time(NULL);
	struct tm t;
	localtime_s(&t, &timer);

	char nowt[128];
	sprintf_s(nowt, "%d_%d_%d_[%d_%d_%d]", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	return nowt;
}