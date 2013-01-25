#include <iostream>
#include "cinder/Rand.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
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
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

	std::vector<std::string> temp;
	std::vector<std::string> words;
	cinder::gl::TextureFont::DrawOptions fontOpts;
	cinder::gl::TextureFontRef font;
	twitCurl twit;

};

void TwitCurlTestApp::prepareSettings( Settings* settings )
{
	settings->setWindowSize(1280, 720);
}

void TwitCurlTestApp::setup()
{
	cinder::gl::clear(Color(0,0,0));
	cinder::gl::enableAlphaBlending(false);
	font = cinder::gl::TextureFont::create( cinder::Font( loadFile("acmesa.TTF"),16));
	std::string resp;
	
	twit.setProxyServerIp(std::string("ip.ip.ip.ip"));
	twit.setProxyServerPort(std::string("port"));

	//setup oauth
	twit.getOAuth().setConsumerKey(std::string("Consumer Key"));
	twit.getOAuth().setConsumerSecret(std::string("Consumer Secret"));
	twit.getOAuth().setOAuthTokenKey(std::string("Access Key"));
	twit.getOAuth().setOAuthTokenSecret(std::string("Access Secret"));
	twit.setTwitterApiType( twitCurlTypes::eTwitCurlApiFormatJson );
	
	if(twit.accountVerifyCredGet() )
	{
		twit.getLastWebResponse( resp );
		cinder::app::console() << resp << std::endl;
		if(twit.search(std::string("perceptualcomputing")))
		{
			twit.getLastWebResponse( resp );
			Json::Value root;
			Json::Reader json;

			bool parsed = json.parse(resp, root, false);
			if(!parsed)
			{
				cinder::app::console() << json.getFormattedErrorMessages() << std::endl;
			}
			else
			{
				const Json::Value results = root["results"];
				for(int i=0;i<results.size();++i)
				{
					temp.clear();
					const std::string content = results[i]["text"].asString();
					temp = cinder::split(content, ' ');
					words.insert(words.end(),temp.begin(),temp.end());
				}
			}
		}
	}
	else
	{
		twit.getLastCurlError( resp );
		cinder::app::console() << resp << std::endl;
	}
}

void TwitCurlTestApp::mouseDown( MouseEvent event )
{
}

void TwitCurlTestApp::update()
{
}

void TwitCurlTestApp::draw()
{

	cinder::gl::color(0,0,0,0.015f);
	cinder::gl::drawSolidRect(cinder::Rectf(0,0,1280,720));

	cinder::gl::color(1,1,1,cinder::Rand::randFloat(0.25f,0.75f));
	int numFrames = cinder::app::getElapsedFrames();
	if(numFrames%15==0)
	{
		if(words.size()>0)
		{
			int i = numFrames%words.size();
			fontOpts.scale(cinder::Rand::randFloat(0.3,3.0f));
			std::string word = words[i];

			font->drawString(word,
							cinder::Vec2f(cinder::Rand::randFloat(cinder::app::getWindowWidth()),
							cinder::Rand::randFloat(cinder::app::getWindowHeight())),
							fontOpts );
		}
	}
}

CINDER_APP_BASIC( TwitCurlTestApp, RendererGl )
