/*
 * tcp.c
 *
 *  Created on: Jun 15, 2011
 *      Author: chenxm
 */
#include <assert.h>
#include <inttypes.h>
#include "order.hpp"
#include <iostream>


// add in order??

/*
 * Search for a continuous segment in order
 */
static Seq *tcp_cont_seq(Seq *lst)
{
    while (lst->next != NULL && lst->nxt_seq == lst->next->seq)
        lst = lst->next;

    return lst;
}

/*
 * reorder the TCP packets
 */
int tcp_order(Order *ord, Seq *new_seq, bool src){
	Seq **plist=NULL, **plist_last=NULL, *pre=NULL, *cp=NULL, *aft=NULL;
	u_int32_t fr=0, bk=0;

	if(src == true){
		plist = &(ord->src);
		plist_last = &(ord->last_src);
	}else{
		plist = &(ord->dst);
		plist_last = &(ord->last_dst);
	}

	if( (*plist) == NULL ){
		/* first packet */
		(*plist) = new_seq;
		(*plist_last) = new_seq;
		ord->num++;
		return 0;
	}else{
		/* set some variables fr,pre,cp,aft,bk to add a new seq to order */
		if((*plist_last)->nxt_seq == new_seq->seq){
			fr = ((*plist_last)->seq);
			pre = (*plist_last);
			cp = tcp_cont_seq((*plist_last));
			aft = cp->next;
			bk = cp->nxt_seq - 1;
		}
		else{
			cp = (*plist);
			/* search position */
			pre = cp;
			while(cp != NULL && (cp->nxt_seq) <= (new_seq->seq)){	/* not (cp->nxt_seq) <= (new_seq->seq) */
				pre = cp;
				cp = cp->next;
			}
			if( cp == NULL){
				/* at list end */
				pre->next = new_seq;
				(*plist_last) = new_seq;
				ord->num++;
				return 0;
			}else{
				fr = pre->seq;				/* start of continuous segment */
				cp = tcp_cont_seq(pre);	 	/* the last seq_t in a continuous segment */
				aft = cp->next;
				bk = cp->nxt_seq - 1;		/* end of continuous segment */
			}
		}
		/* add new seq to the right position */
		if((new_seq->seq) >= fr && (new_seq->seq) <= bk ){
			if( (new_seq->nxt_seq -1) <= bk){
				/* retransmission */
				delete new_seq;
				return 1;
			}else{
				/* adjust the packet */
				u_int32_t	delta = (new_seq->nxt_seq-1) - bk;
				if(new_seq->pkt != NULL){
					if(new_seq->pkt->tcp_data != NULL){				/* check if we store the packet payload */
						new_seq->pkt->tcp_data = new_seq->pkt->tcp_data + (new_seq->pkt->tcp_dl - delta);
					}
					new_seq->pkt->tcp_dl = delta;
				}
				new_seq->seq = bk + 1;
				/* update order */
				cp->next = new_seq;
				(*plist_last) = new_seq;
				if( aft != NULL ){
					new_seq->next = aft;
					/* Connect with follower */
					if(new_seq->nxt_seq <= aft->seq){
						new_seq->next = aft;
						(*plist_last) = new_seq;
						ord->num++;
						return 0;
					}else{
						/* partially overlapped */
						if(new_seq->pkt != NULL){
							new_seq->pkt->tcp_dl -= (new_seq->nxt_seq - aft->seq);
						}
						new_seq->nxt_seq = aft->seq;
						new_seq->next = aft;
						(*plist_last) = new_seq;
						ord->num++;
						return 0;
					}
				}

				return 0;
			}
		}else{
			cp->next = new_seq;

			if(aft != NULL){
				if(new_seq->nxt_seq <= aft->seq){
					new_seq->next = aft;
					(*plist_last) = new_seq;
					ord->num++;
					return 0;
				}else{
					/* partially overlapped */
					if(new_seq->pkt != NULL){
						new_seq->pkt->tcp_dl -= (new_seq->nxt_seq - aft->seq);
					}
					new_seq->nxt_seq = aft->seq;
					new_seq->next = aft;
					(*plist_last) = new_seq;
					ord->num++;
					return 0;
				}
			}

			return 0;
		}
	}
}

/* for debugging */
/* check if TCP is ordered correctly */
int tcp_order_check(Order *order){
	//u_int32_t seq, next_seq;
	int src_check = 0, dst_check = 0;
	Seq	*ps;

	if(order == NULL){
		return 0;
	}

	ps = order->src;
	if(ps == NULL){
		src_check = 1;
	}else{
		while(ps->next != NULL){
			if(ps->nxt_seq == ps->next->seq){
				ps = ps->next;
			}else{
				printf("%"PRIu32"\t%"PRIu32"\n", ps->nxt_seq, ps->next->seq);
				src_check = 0;
				break;
			}
		}
		if(ps->next == NULL){
			src_check = 1;
		}
	}

	ps = order->dst;
	if( ps == NULL){
		dst_check = 1;
	}else{
		while(ps->next != NULL){
			if(ps->nxt_seq == ps->next->seq){
				ps = ps->next;
			}else{
				printf("%"PRIu32"\t%"PRIu32"\n", ps->nxt_seq, ps->next->seq);
				dst_check = 0;
				break;
			}
		}
		if(ps->next == NULL){
			dst_check = 1;
		}
	}

	if(src_check == 1 && dst_check == 1){
		return true;
	}
	return false;
}
