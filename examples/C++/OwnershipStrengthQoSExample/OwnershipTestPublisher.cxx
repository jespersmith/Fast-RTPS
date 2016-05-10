/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This generated file is licensed to you under the terms described in the
 * fastcdr_LICENSE file included in this fastcdr distribution.
 *
 *************************************************************************
 * 
 * @file OwnershipTestPublisher.cpp
 * This file contains the implementation of the publisher functions.
 *
 * This file was generated by the tool fastcdrgen.
 */

#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/Domain.h>
#include <fastrtps/utils/eClock.h>
#include <sstream>

#include "OwnershipTestPublisher.h"

OwnershipTestPublisher::OwnershipTestPublisher() 
   :mp_participant(nullptr), 
    mp_publisher(nullptr), 
    m_strength(DefaultStrength) 
    {}

OwnershipTestPublisher::~OwnershipTestPublisher() {	Domain::removeParticipant(mp_participant);}

void OwnershipTestPublisher::setOwnershipStrength(unsigned int strength)
{
   m_strength = strength;
}

bool OwnershipTestPublisher::init()
{
	// Create RTPSParticipant
	
	ParticipantAttributes PParam;
	PParam.rtps.builtin.domainId = 0;
	PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
	PParam.rtps.setName("Participant_publisher");
   mp_participant = Domain::createParticipant(PParam);
	if(mp_participant == nullptr)
		return false;
	
	//Register the type
	
	Domain::registerType(mp_participant,(TopicDataType*) &myType);
	
	// Create Publisher
	
	PublisherAttributes Wparam;
	Wparam.topic.topicKind = NO_KEY;
	Wparam.topic.topicDataType = myType.getName();  //This type MUST be registered
	Wparam.topic.topicName = "OwnershipTestPubSubTopic";
	mp_publisher = Domain::createPublisher(mp_participant,Wparam,(PublisherListener*)&m_listener);
	if(mp_publisher == nullptr)
		return false;
	cout << "Publisher created, waiting for Subscribers." << endl;
	return true;
}

void OwnershipTestPublisher::PubListener::onPublicationMatched(Publisher* /*pub*/,MatchingInfo& info)
{
	if (info.status == MATCHED_MATCHING)
	{
		n_matched++;
		cout << "Publisher matched" << endl;
	}
	else
	{
		n_matched--;
		cout << "Publisher unmatched" << endl;
	}
}

void OwnershipTestPublisher::run()
{
	while(m_listener.n_matched == 0)
	{
		eClock::my_sleep(250); // Sleep 250 ms
	}
	
	char ch = 'y';
	do
	{
		if(ch == 'y')
		{
         ExampleMessage st;
         std::stringstream ss;
         ss << "Hello with strength " << m_strength;

         st.message(ss.str());
         st.ownershipStrength(m_strength);
         st.index(m_messagesSent);
			mp_publisher->write(&st);  

         m_messagesSent++;
			cout << "Sending message, index = " << m_messagesSent << " with strength " << m_strength << ", send another sample?(y-yes,n-stop): ";
		}
		else if(ch == 'n')
		{
			cout << "Stopping execution " << endl;
			break;
		}
		else
		{
			cout << "Command " << ch << " not recognized, please enter \"y/n\":";
		}

	} while(std::cin >> ch);
}
