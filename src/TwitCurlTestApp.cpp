#include <iostream>
#include "cinder/Rand.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "cinder/gl/TextureFont.h"
#include "json/json.h"
#include "twitcurl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TwitCurlTestApp : public AppBasic {
  public:
	void prepareSettings( Settings* settings );
	void setup();
	void update();
	void draw();

	vector<string> temp;
	vector<string> words;
	gl::TextureFont::DrawOptions fontOpts;
	gl::TextureFontRef font;
	twitCurl twit;

};

void TwitCurlTestApp::prepareSettings( Settings* settings )
{
	settings->setWindowSize(1280, 720);
}

void TwitCurlTestApp::setup()
{
	gl::clear(Color(0,0,0));
	gl::enableAlphaBlending(false);
	font = gl::TextureFont::create( Font( loadFile("acmesa.TTF"),16));
	string resp;
	
	twit.setProxyServerIp(string("ip.ip.ip.ip"));
	twit.setProxyServerPort(string("port"));

	//setup oauth
	twit.getOAuth().setConsumerKey(string("Consumer Key"));
	twit.getOAuth().setConsumerSecret(string("Consumer Secret"));
	twit.getOAuth().setOAuthTokenKey(string("Access Key"));
	twit.getOAuth().setOAuthTokenSecret(string("Access Secret"));
	twit.setTwitterApiType( twitCurlTypes::eTwitCurlApiFormatJson );
	
	if(twit.accountVerifyCredGet() )
	{
		twit.getLastWebResponse( resp );
		console() << resp << std::endl;
		if(twit.search(string("perceptualcomputing")))
		{
			twit.getLastWebResponse( resp );
			Json::Value root;
			Json::Reader json;

			bool parsed = json.parse(resp, root, false);
			if(!parsed)
			{
				console() << json.getFormattedErrorMessages() << std::endl;
			}
			else
			{
				const Json::Value results = root["results"];
				for(int i=0;i<results.size();++i)
				{
					temp.clear();
					const string content = results[i]["text"].asString();
					temp = split(content, ' ');
					words.insert(words.end(),temp.begin(),temp.end());
				}
			}
		}
	}
	else
	{
		twit.getLastCurlError( resp );
		console() << resp << std::endl;
	}
}

void TwitCurlTestApp::update()
{
}

void TwitCurlTestApp::draw()
{

	gl::color(0,0,0,0.015f);
	gl::drawSolidRect(cinder::Rectf(0,0,getWindowWidth(),getWindowHeight()));

	gl::color(1,1,1,Rand::randFloat(0.25f,0.75f));
	int numFrames = getElapsedFrames();
	if(numFrames%15==0)
	{
		if(words.size()>0)
		{
			int i = numFrames%words.size();
			fontOpts.scale(Rand::randFloat(0.3,3.0f));
			string word = words[i];

			font->drawString(word, Vec2f(Rand::randFloat(getWindowWidth()), Rand::randFloat(getWindowHeight())), fontOpts);
		}
	}
}

CINDER_APP_BASIC( TwitCurlTestApp, RendererGl )
