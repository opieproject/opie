#ifndef IMAPRESPONSE_H
#define IMAPRESPONSE_H

#include <qobject.h>

#define MAX_BODYPARTS 		50

class IMAPHandler;

class IMAPResponseEnums
{
public:
	enum IMAPResponseStatus {
		OK, NO, BAD, PREAUTH, BYE, UnknownStatus = 0
	};

	enum IMAPResponseFlags {
		Seen, Answered, Flagged, Deleted, Draft, Recent,
		Noinferiors, Noselect, Marked, Unmarked, UnknownFlag = 0
	};

	enum IMAPResponseCode {
		ALERT, NEWNAME, PARSE, PERMANENTFLAGS, READONLY, READWRITE,
		TRYCREATE, UIDVALIDITY, UNSEEN, NoCode, UnknownCode = 0
	};

};

class IMAPResponseAddress
{
public:
	IMAPResponseAddress(QString name = 0, QString adl = 0, QString mailbox = 0, QString host = 0)
	{
		_name = name;
		_adl = adl;
		_mailbox = mailbox;
		_host = host;
	}

	void setName(QString name) { _name = name; }
	QString name() { return _name; }
	void setAdl(QString adl) { _adl = adl; }
	QString adl() { return _adl; }
	void setMailbox(QString mailbox) { _mailbox = mailbox; }
	QString mailbox() { return _mailbox; }
	void setHost(QString host) { _host = host; }
	QString host() { return _host; }

	QString toString() 
	{
		if (_name.isNull() && _mailbox.isNull() && _host.isNull())
			return QString(0);
		if (_name != _mailbox + "@" + _host) 
			return _name + " <" + _mailbox + "@" + _host + ">";
		else
			return _name;
	}

	QString email() 
	{
		if (_host.isEmpty()) return _mailbox;
		else return _mailbox + "@" + _host;
	}

private:
	QString _name, _adl, _mailbox, _host;

};

class IMAPResponseAddressList : public QValueList<IMAPResponseAddress>
{
public:
	IMAPResponseAddressList()
		: QValueList<IMAPResponseAddress>()
	{

	}

	QString toString()
	{
		QString string;
		IMAPResponseAddressList::Iterator it;
		for (it = this->begin(); it != this->end(); it++) {
			QString tmp = (*it).toString();
			if (!tmp.isNull() && string.isEmpty()) string = tmp;
			else if (!tmp.isNull()) string += ", " + tmp;
		}
		return string;
	}

};

class IMAPResponseBody
{
public:
	void setData(QString data) { _data = data; }
	QString data() { return _data; }

private:
	QString _data;

};

class IMAPResponseBodyPart : public IMAPResponseBody
{
public:
	void setPart(QString part) { _part = part; }
	QString part() { return _part; }

private:
	QString _part;

};

class IMAPResponseEnvelope
{
public:
	void setMailDate(QString date) { _date = date; }
	QString mailDate() { return _date; }
	void setSubject(QString subject) { _subject = subject; }
	QString subject() { return _subject; }
	void addFrom(IMAPResponseAddress from) { _from.append(from); }
	IMAPResponseAddressList from() { return _from; }
	void addSender(IMAPResponseAddress sender) { _sender.append(sender); }
	IMAPResponseAddressList sender() { return _sender; }
	void addReplyTo(IMAPResponseAddress replyTo) { _replyTo.append(replyTo); }
	IMAPResponseAddressList replyTo() { return _replyTo; }
	void addTo(IMAPResponseAddress to) { _to.append(to); }
	IMAPResponseAddressList to() { return _to; }
	void addCc(IMAPResponseAddress cc) { _cc.append(cc); }
	IMAPResponseAddressList cc() { return _cc; }
	void addBcc(IMAPResponseAddress bcc) { _bcc.append(bcc); }
	IMAPResponseAddressList bcc() { return _bcc; }
	void setInReplyTo(QString inReplyTo) { _inReplyTo = inReplyTo; }
	QString inReplyTo() { return _inReplyTo; }
	void setMessageId(QString messageId) { _messageId = messageId; }
	QString messageId() { return _messageId; }

private:
	QString _date, _subject, _inReplyTo, _messageId;
	IMAPResponseAddressList _from, _sender, _replyTo, _to, _cc, _bcc;

};

class IMAPResponseStatusResponse : public IMAPResponseEnums
{
public:
	IMAPResponseStatusResponse(IMAPResponseStatus status = (IMAPResponseStatus)0, QString comment = 0, IMAPResponseCode code = (IMAPResponseCode)0)
	{
		_status = status;
		_comment = comment;
		_code = code;
	}

	void setStatus(IMAPResponseStatus status) { _status = status; }
	IMAPResponseStatus status() { return _status; }
	void setComment(QString comment) { _comment = comment; }
	QString comment() { return _comment; }
	void setResponseCode(IMAPResponseCode code) { _code = code; }
	IMAPResponseCode responseCode() { return _code; }

	void setExitedUnexpected(bool exitu) { _exitu = exitu; }
	bool exitedUnexpected() { return _exitu; }

private:
	IMAPResponseStatus _status;
	QString _comment;
	IMAPResponseCode _code;
	bool _exitu;

};

class IMAPResponseOK : public IMAPResponseEnums
{
public:
	IMAPResponseOK(QString comment = 0, IMAPResponseCode code = (IMAPResponseCode)0)
	{
		_comment = comment;
		_code = code;
	}

	void setComment(QString comment) { _comment = comment; }
	QString comment() { return _comment; }
	void setResponseCode(IMAPResponseCode code) { _code = code; }
	IMAPResponseCode responseCode() { return _code; }

private:
	QString _comment;
	IMAPResponseCode _code;

};

class IMAPResponseNO : public IMAPResponseEnums
{
public:
	IMAPResponseNO(QString comment = 0, IMAPResponseCode code = (IMAPResponseCode)0)
	{
		_comment = comment;
		_code = code;
	}

	void setComment(QString comment) { _comment = comment; }
	QString comment() { return _comment; }
	void setResponseCode(IMAPResponseCode code) { _code = code; }
	IMAPResponseCode responseCode() { return _code; }

private:
	QString _comment;
	IMAPResponseCode _code;

};

class IMAPResponseBAD : public IMAPResponseEnums
{
public:
	IMAPResponseBAD(QString comment = 0, IMAPResponseCode code = (IMAPResponseCode)0)
	{
		_comment = comment;
		_code = code;
	}

	void setComment(QString comment) { _comment = comment; }
	QString comment() { return _comment; }
	void setResponseCode(IMAPResponseCode code) { _code = code; }
	IMAPResponseCode responseCode() { return _code; }

private:
	QString _comment;
	IMAPResponseCode _code;

};

class IMAPResponseCAPABILITY
{
public:
	IMAPResponseCAPABILITY(QStringList capabilities = 0)
	{
		_capabilities = capabilities;
	}

	void setCapabilities(QStringList capabilities) { _capabilities = capabilities; }
	QStringList capabilities() { return _capabilities; }

private:
	QStringList _capabilities;

};

class IMAPResponseLIST : public IMAPResponseEnums
{
public:
	IMAPResponseLIST(QValueList<IMAPResponseFlags> flags = QValueList<IMAPResponseFlags>(), QString folderSeparator = 0, QString folder = 0)
	{
		_flags = flags;
		_folderSeparator = folderSeparator;
		_folder = folder;
	}

	void setFlags(QValueList<IMAPResponseFlags> flags) { _flags = flags; }
	QValueList<IMAPResponseFlags> flags() { return _flags; }
	void setFolderSeparator(QString folderSeparator) { _folderSeparator = folderSeparator; }
	QString folderSeparator() { return _folderSeparator; }
	void setFolder(QString folder) { _folder = folder; }
	QString folder() { return _folder; }

private:
	QValueList<IMAPResponseFlags> _flags;
	QString _folderSeparator, _folder;

};

class IMAPResponseLSUB : public IMAPResponseEnums
{
public:
	IMAPResponseLSUB(QValueList<IMAPResponseFlags> flags = QValueList<IMAPResponseFlags>(), QString folderSeperator = 0, QString folder = 0)
	{
		_flags = flags;
		_folderSeperator = folderSeperator;
		_folder = folder;
	}

	void setFlags(QValueList<IMAPResponseFlags> flags) { _flags = flags; }
	QValueList<IMAPResponseFlags> flags() { return _flags; }
	void setFolderSeperator(QString folderSeperator) { _folderSeperator = folderSeperator; }
	QString folderSeperator() { return _folderSeperator; }
	void setFolder(QString folder) { _folder = folder; }
	QString folder() { return _folder; }

private:
	QValueList<IMAPResponseFlags> _flags;
	QString _folderSeperator, _folder;

};

class IMAPResponseSTATUS
{
public:
	IMAPResponseSTATUS(QString mailbox = 0, QString messages = 0, QString recent = 0, QString uidnext = 0, QString uidvalidity = 0, QString unseen = 0)
	{
		_mailbox = mailbox;
		_messages = messages;
		_recent = recent;
		_uidnext = uidnext;
		_uidvalidity = uidvalidity;
		_unseen = unseen;
	}

	void setMailbox(QString &mailbox) { _mailbox = mailbox; }
	QString mailbox() { return _mailbox; }
	void setMessages(QString &messages) { _messages = messages; }
	QString messages() { return _messages; }
	void setRecent(QString &recent) { _recent = recent; }
	QString recent() { return _recent; }
	void setUidnext(QString &uidnext) { _uidnext = uidnext; }
	QString uidnext() { return _uidnext; }
	void setUidvalidity(QString &uidvalidity) { _uidvalidity = uidvalidity; }
	QString uidvalidity() { return _uidvalidity; }
	void setUnseen(QString &unseen) { _unseen = unseen; }
	QString unseen() { return _unseen; }

private:
	QString _mailbox, _messages, _recent, _uidnext, _uidvalidity, _unseen;

};

class IMAPResponseSEARCH
{
public:
	IMAPResponseSEARCH(QStringList mails = 0)
	{
		_mails = mails;
	}

	void setMails(QStringList mails) { _mails = mails; }
	QStringList mails() { return _mails; }

private:
	QStringList _mails;

};

class IMAPResponseFLAGS : public IMAPResponseEnums
{
public:
	IMAPResponseFLAGS(QValueList<IMAPResponseFlags> flags = QValueList<IMAPResponseFlags>())
	{
		_flags = flags;
	}

	void setFlags(QValueList<IMAPResponseFlags> flags) { _flags = flags; }
	QValueList<IMAPResponseFlags> flags() { return _flags; }

private:
	QValueList<IMAPResponseFlags> _flags;

};

class IMAPResponseEXISTS
{
public:
	IMAPResponseEXISTS(QString mails = 0)
	{
		_mails = mails;
	}

	void setMails(QString mails) { _mails = mails; }
	QString mails() { return _mails; }

private:
	QString _mails;

};

class IMAPResponseRECENT
{
public:
	IMAPResponseRECENT(QString mails = 0)
	{
		_mails = mails;
	}

	void setMails(QString mails) { _mails = mails; }
	QString mails() { return _mails; }

private:
	QString _mails;

};

class IMAPResponseEXPUNGE 
{
public:
	IMAPResponseEXPUNGE(QString mails = 0)
	{
		_mails = mails;
	}

	void setMails(QString mails) { _mails = mails; }
	QString mails() { return _mails; }

private:
	QString _mails;

};

class IMAPResponseFETCH : public IMAPResponseEnums
{
public:
	void setEnvelope(IMAPResponseEnvelope envelope) { _envelope = envelope; }
	IMAPResponseEnvelope envelope() { return _envelope; }
	void setFlags(QValueList<IMAPResponseFlags> flags) { _flags = flags; }
	QValueList<IMAPResponseFlags> flags() { return _flags; }
	void setInternalDate(QString idate) { _idate = idate; }
	QString internalDate() { return _idate; }
	void setRFC822Size(QString rfc822size) { _rfc822size = rfc822size; }
	QString RFC822Size() { return _rfc822size; }
	void setUid(QString uid) { _uid = uid; }
	QString uid() { return _uid; }

	void setBody(QString body) { _body = body; }
	QString body() { return _body; }
	void addBodyPart(IMAPResponseBodyPart part) { _bodyParts.append(part); }
	void setBodyParts(QValueList<IMAPResponseBodyPart> parts) { _bodyParts = parts; }
	QValueList<IMAPResponseBodyPart> bodyParts() { return _bodyParts; }
	IMAPResponseBodyPart bodyPart(int part) { return _bodyParts[part]; }
	void setRfc822(QString rfc822) { _rfc822 = rfc822; }
	QString rfc822() { return _rfc822; }
	void setRfc822Header(QString rfc822Header) { _rfc822Header = rfc822Header; }
	QString rfc822Header() { return _rfc822Header; }
	void setRfc822Text(QString rfc822Text) { _rfc822Text = rfc822Text; }
	QString rfc822Text() { return _rfc822Text; }

private:
	IMAPResponseEnvelope _envelope;
	QValueList<IMAPResponseFlags> _flags;
	QString _idate, _rfc822size, _uid, _body, _rfc822, _rfc822Header, _rfc822Text;
	QValueList<IMAPResponseBodyPart> _bodyParts;

};

class IMAPResponse : public IMAPResponseEnums
{
public:
	void setTag(QString tag) { _tag = tag; }
	QString tag() { return _tag; }

	void setImapHandler(IMAPHandler *handler) { _handler = handler; }
	IMAPHandler *imapHandler() { return _handler; }

	void setStatusResponse(IMAPResponseStatusResponse response) { _response = response; }
	IMAPResponseStatusResponse statusResponse() { return _response; }

	void addOK(IMAPResponseOK ok) { _okl.append(ok); }
	void addNO(IMAPResponseNO no) { _nol.append(no); }
	void addBAD(IMAPResponseBAD bad) { _badl.append(bad); }
	void addCAPABILITY(IMAPResponseCAPABILITY capability) { _capabilityl.append(capability); }
	void addLIST(IMAPResponseLIST list) { _listl.append(list); }
	void addLSUB(IMAPResponseLSUB lsub) { _lsubl.append(lsub); }
	void addSTATUS(IMAPResponseSTATUS status) { _statusl.append(status); }
	void addSEARCH(IMAPResponseSEARCH search) { _searchl.append(search); }
	void addFLAGS(IMAPResponseFLAGS flags) { _flagsl.append(flags); }
	void addEXISTS(IMAPResponseEXISTS exists) { _existsl.append(exists); }
	void addRECENT(IMAPResponseRECENT recent) { _recentl.append(recent); }
	void addEXPUNGE(IMAPResponseEXPUNGE expunge) { _expungel.append(expunge); }
	void addFETCH(IMAPResponseFETCH fetch) { _fetchl.append(fetch); }

	QValueList<IMAPResponseOK> OK() { return _okl; }
	QValueList<IMAPResponseNO> NO() { return _nol; }
	QValueList<IMAPResponseBAD> BAD() { return _badl; }
	QValueList<IMAPResponseCAPABILITY> CAPABILITY() { return _capabilityl; }
	QValueList<IMAPResponseLIST> LIST() { return _listl; }
	QValueList<IMAPResponseLSUB> LSUB() { return _lsubl; }
	QValueList<IMAPResponseSTATUS> STATUS() { return _statusl; }
	QValueList<IMAPResponseSEARCH> SEARCH() { return _searchl; }
	QValueList<IMAPResponseFLAGS> FLAGS() { return _flagsl; }
	QValueList<IMAPResponseEXISTS> EXISTS() { return _existsl; }
	QValueList<IMAPResponseRECENT> RECENT() { return _recentl; }
	QValueList<IMAPResponseEXPUNGE> EXPUNGE() { return _expungel; }
	QValueList<IMAPResponseFETCH> FETCH() { return _fetchl; }

private:
	QString _tag;
	IMAPResponseStatusResponse _response;

	IMAPHandler *_handler;

	QValueList<IMAPResponseOK> _okl;
	QValueList<IMAPResponseNO> _nol;
	QValueList<IMAPResponseBAD> _badl;
	QValueList<IMAPResponseCAPABILITY> _capabilityl;
	QValueList<IMAPResponseLIST> _listl;
	QValueList<IMAPResponseLSUB> _lsubl;
	QValueList<IMAPResponseSTATUS> _statusl;
	QValueList<IMAPResponseSEARCH> _searchl;
	QValueList<IMAPResponseFLAGS> _flagsl;
	QValueList<IMAPResponseEXISTS> _existsl;
	QValueList<IMAPResponseRECENT> _recentl;
	QValueList<IMAPResponseEXPUNGE> _expungel;
	QValueList<IMAPResponseFETCH> _fetchl;

};

class IMAPResponseParser : public QObject, public IMAPResponseEnums 
{
	Q_OBJECT

public:
	IMAPResponseParser(const QString &data);

	IMAPResponse response();

signals:
	void needMoreData(const QString &comment);

protected:
	void parseResponse(const QString &data, bool tagged = false);

	QStringList splitData(const QString &data, bool withBrackets);
	void parseParenthesizedList(const QString &data, QStringList &parsed);
	void splitTagData(const QString &line, QString &tag, QString &data);
	QString removeLimiters(QString &string, const QChar &sl = '"', const QChar &el = '"');
	IMAPResponseCode getResponseCode(const QString &line);
	QValueList<IMAPResponseFlags> parseFlagList(const QStringList &flags);

private:
	IMAPResponse _iresponse;

};

#endif

