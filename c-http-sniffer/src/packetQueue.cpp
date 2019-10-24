#include "packetQueue.hpp"

PacketQueue::PacketQueue()
	: pkt_first(NULL), pkt_last(NULL), que_len(0)
{
	pthread_mutex_init(&pkt_queue_mutex, NULL);
}

void PacketQueue::enq(Packet *pkt)
{
	pthread_mutex_lock(&pkt_queue_mutex);
	if(que_len == 0){
		pkt_first = pkt;
		pkt_last = pkt;
		pkt_last->next = NULL;
		que_len++;
		pthread_mutex_unlock(&pkt_queue_mutex);
		return;
	}
	pkt_last->next = pkt;
	pkt_last = pkt;
	pkt_last->next = NULL;
	que_len++;
	pthread_mutex_unlock(&pkt_queue_mutex);
}

Packet* PacketQueue::deq()
{
	pthread_mutex_lock(&pkt_queue_mutex);
	Packet* pkt;
	if(que_len == 0)
	{
		pthread_mutex_unlock(&pkt_queue_mutex);
		return NULL;
	}
	else if (que_len == 1 )
	{
		pkt_last = NULL;
	}
	pkt = pkt_first;
	pkt_first = pkt_first->next;
	que_len--;
	pthread_mutex_unlock(&pkt_queue_mutex);
	return pkt;
}

void PacketQueue::clr()
{
	pthread_mutex_lock(&pkt_queue_mutex);
	Packet *pkt;
	while(que_len > 0)
	{
		pkt = pkt_first;
		pkt_first = pkt_first->next;
		delete pkt;
		que_len--;
	}
	pkt_first =  NULL;
	pkt_last = NULL;
	que_len = 0;
	pthread_mutex_unlock(&pkt_queue_mutex);
}

unsigned int PacketQueue::len()
{
	return que_len;
}

void PacketQueue::print()
{
	printf("(Packet queue length)%d\n", len());
}