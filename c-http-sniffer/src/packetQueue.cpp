#include "packetQueue.hpp"

PacketQueue::PacketQueue() : pkt_first(NULL), pkt_last(NULL), que_len(0) {
	pthread_mutex_init(&mutex, NULL);
}

bool PacketQueue::enq(packet_t *pkt) {
	pthread_mutex_lock(&mutex);
	if(que_len == 0)
	{
		pkt_first = pkt;
		pkt_last = pkt;
		pkt_last->next = NULL;
		que_len++;
		pthread_mutex_unlock(&mutex);
		return true;
	}

	pkt_last->next = pkt;
	pkt_last = pkt;
	pkt_last->next = NULL;
	que_len++;
	pthread_mutex_unlock(&mutex);
	return true;
}

packet_t * PacketQueue::deq(void) {
	pthread_mutex_lock(&mutex);
	packet_t *pkt;
	if(que_len == 0)
	{
		pthread_mutex_unlock(&mutex);
		return NULL;
	}
	else if (que_len == 1 )
	{
		pkt_last = NULL;
	}
	pkt = pkt_first;
	pkt_first = pkt_first->next;
	que_len--;
	pthread_mutex_unlock(&mutex);
	return pkt;
}

bool PacketQueue::clr(void) {
	pthread_mutex_lock(&mutex);
	packet_t *pkt;
	while(que_len > 0)
	{
		pkt = pkt_first;
		pkt_first = pkt_first->next;
		packet_free(pkt);
		que_len--;
	}
	pkt_first =  NULL;
	pkt_last = NULL;
	que_len = 0;
	pthread_mutex_unlock(&mutex);
	return true;
}

unsigned int PacketQueue::len() {
	return que_len;
}

void PacketQueue::print(void) {
	printf("(Packet queue length)%d\n", len());
}