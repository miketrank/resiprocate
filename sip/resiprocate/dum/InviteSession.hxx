#if !defined(RESIP_INVITESESSION_HXX)
#define RESIP_INVITESESSION_HXX

#include "resiprocate/dum/DialogUsage.hxx"
#include "resiprocate/SipMessage.hxx"

namespace resip
{

class SdpContents;

class InviteSession : public DialogUsage
{
   public:

      virtual void send(SipMessage& msg);

      /// Called to set the offer that will be used in the next messages that
      /// sends and offer. Does not send an offer 
      virtual void setOffer(const SdpContents* offer);
      
      /// Called to set the answer that will be used in the next messages that
      /// sends an offer. Does not send an answer
      virtual void setAnswer(const SdpContents* answer);

      /// Completes the three way handshake.  In some circumstances(not re-invite), setAnswer
      /// must have been called or else this will throw(3261 13.2.1)
      virtual SipMessage& ackConnection();

      /// Makes the dialog end. Depending on the current state, this might
      /// results in BYE or CANCEL being sent.
      virtual SipMessage& end();

      /// Rejects an offer at the SIP level. So this can send a 487 !dcm! --
      /// should be 488? to a reinvite INVITE or an UPDATE
      virtual SipMessage& rejectOffer(int statusCode);
      
      //accept a re-invite, etc.  Always 200?
      virtual SipMessage& acceptOffer(int statusCode = 200);

      
      // If the app has called setOffer prior to targetRefresh, the reINVITE
      // will contain the proposed offer. If the peer supports UPDATE, always
      // prefer UPDATE over reINVITE (no 3-way handshake required)
      // !jf! there are more things you could update in the targetRefresh 
      //!dcm! -- the UPDATE rfc states that re-invite should be preferred on 
      // established dialogs, in case user approval is required.
      virtual SipMessage& targetRefresh(const NameAddr& localUri);

      //always does re-invite for now...ACK is hidden
      //call setOffer or setAnswer bfore calling these.
      //calling answerModifySession /wout setAnswer is invalid
      virtual SipMessage& modifySession();
 
      virtual SipMessage& makeRefer(const NameAddr& referTo);

      const SdpContents* getLocalSdp();
      const SdpContents* getRemoteSdp();

   public:
      virtual void dispatch(const SipMessage& msg);
      virtual void dispatch(const DumTimeout& timer);

      typedef Handle<InviteSession> InviteSessionHandle;
      InviteSessionHandle getSessionHandle();

      typedef enum
      {
         None, // means no Offer or Answer (may have SDP)
         Offer,
         Answer
      } OfferAnswerType;

   protected:
      typedef enum
      {
         Nothing,
         Offerred,
         Answered, // agreed
         CounterOfferred
      } OfferState;

      // If sdp==0, the offer was rejected
      void incomingSdp(const SipMessage& msg, const SdpContents* sdp);

      // If sdp==0, the offer is being rejected
      void sendSdp(SdpContents* sdp);

      std::pair<OfferAnswerType, const SdpContents*> getOfferOrAnswer(const SipMessage& msg) const;
      
      typedef enum
      {
         Initial,  // No session setup yet
         Early,    
         Proceeding,
         Accepting, 
         Cancelled,
         Connected,
         Terminated,
         ReInviting,
         AcceptingReInvite
      } State;

      State mState;

      InviteSession(DialogUsageManager& dum, Dialog& dialog, State initialState);
      void copyAuthorizations(SipMessage& request);
      void makeAck();

      OfferState mOfferState;
      SdpContents* mCurrentLocalSdp;
      SdpContents* mCurrentRemoteSdp;
      SdpContents* mProposedLocalSdp;
      SdpContents* mProposedRemoteSdp;
      SdpContents* mNextOfferOrAnswerSdp;

      SipMessage mLastRequest;
      SipMessage mLastResponse;
      //used to cache 200 to Invite
      SipMessage mFinalResponse;      
      SipMessage mAck;

      ~InviteSession();

   private:
      friend class Dialog;      
      friend class DialogUsageManager;

      unsigned long mCurrentRetransmit200;      
      static unsigned long T1;
      static unsigned long T2;
      static unsigned long TimerH;

      // disabled
      InviteSession(const InviteSession&);
      InviteSession& operator=(const InviteSession&);
};

}

#endif

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */
