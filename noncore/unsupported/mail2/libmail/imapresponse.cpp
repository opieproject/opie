#include "imapresponse.h"


IMAPResponseParser::IMAPResponseParser()
{
}

void IMAPResponseParser::parse ( const QString &_data )
{	
	QString data = _data;

	int pos = 0;
	int len = data. length ( );
	
	
	while ( pos < len ) {
		pos = data. find ( QRegExp ( "[^\\s]" ), pos );
		
		if (( pos < 0 ) ||  ( pos >= len ))
			break;
	
		switch ( data [pos]. latin1 ( )) {
			case '*': {
				qDebug ( "* ASTERIX\n" );
			
				int eol = data. find ( '\n', pos );
				int bracket = data. findRev ( '{', eol );
				int rest = data. find ( QRegExp ( "[^\\s]" ), pos + 1 );
				
				qDebug ( "pos=%d, rest=%d, bracket=%d, eol=%d\n", pos, rest, bracket, eol );
				
				if ( bracket > pos ) {
					uint needdata = data. mid ( bracket + 1, data. find ( '}', bracket + 1 ) - bracket - 1 ). toUInt ( );
					
					if ( needdata ) {
						qDebug ( "nd=%d - hd=%d\n", needdata, ( len - eol - 1 ));
					
						while ( needdata > ( len - eol - 1 )) {
							qDebug ( "emitting need more...\n" );
							emit needMoreData ( data );
							len = data. length ( );
						}
						qDebug ( "Got all data...\n" );
					
						QString tmp = data. mid ( rest, eol - rest + 1 + needdata );
						
						int tail = 0;	
						
						while ( data [eol - rest + 1 + needdata + tail] != ')' )
							tail++;
						tmp. append ( data. mid ( eol - rest + 1 + needdata, tail + 1 ));
							
					
						qDebug ( "Complete parse = |%s|\n", tmp.latin1());
					
						parseResponse ( tmp );
						
						pos = rest + needdata + tail + 1;
						break;
					}									
				}
				
				parseResponse ( data. mid ( rest, eol - rest + 1 ). stripWhiteSpace ( ));				
				break;
			}          
			case '+': {
				qDebug ( "+ PLUS\n" );
				
				emit needMoreData ( data );
				len = data. length ( );
				break;
			}			          
			default : {
				qDebug ( "OTHER: '%s...'\n", data. mid ( pos, 20 ). latin1 ( ));
			
				uint rest = data. find ( ' ', pos + 1 );
				rest = data. find ( QRegExp ( "[^\\s]" ), rest + 1 );
				_iresponse. setTag ( data. mid ( pos, rest - pos ). stripWhiteSpace ( ));
				parseResponse ( data. mid ( rest, data. find ( '\n', rest )). stripWhiteSpace ( ), true );
				break;
			}
		}

		// skip to end-of-line
		while (( pos < len ) && ( data [pos] != '\n' ))
			pos++;
	}
}


IMAPResponse IMAPResponseParser::response()
{
	return _iresponse;
}

void IMAPResponseParser::parseResponse(const QString &data, bool tagged)
{
	QString response, line;
	int pos;
	bool isNum = false;


//	qDebug ( "\n\n#### PRD #### : #%s#\n\n", data.latin1());

	if ((pos = data.find(' ')) != -1) {
		response = data.left(pos).upper();
		response.toInt(&isNum);
		line = data.right(data.length() - pos - 1);
	} else {
		qWarning("IMAPResponseParser: parseResponse: No response found.");
		return;
	}

	if (response == "OK" && tagged) {
		IMAPResponseStatusResponse status(OK, line);
		status.setResponseCode(getResponseCode(status.comment()));
		_iresponse.setStatusResponse(status);
	} else if (response == "OK" && !tagged) {
		IMAPResponseOK ok(line, getResponseCode(line));
		_iresponse.addOK(ok);
	} else if (response == "NO" && tagged) {
		IMAPResponseStatusResponse status(NO, line);
		status.setResponseCode(getResponseCode(status.comment()));
		_iresponse.setStatusResponse(status);
	} else if (response == "NO" && !tagged) {
		IMAPResponseNO no(line, getResponseCode(line));
		_iresponse.addNO(no);
	} else if (response == "BAD" && tagged) {
		IMAPResponseStatusResponse status(BAD, line);
		status.setResponseCode(getResponseCode(status.comment()));
		_iresponse.setStatusResponse(status);
	} else if (response == "BAD" && !tagged) {
		IMAPResponseBAD bad(line, getResponseCode(line));
		_iresponse.addBAD(bad);
	} else if (response == "PREAUTH" && tagged) {
		IMAPResponseStatusResponse status(PREAUTH, line);
		_iresponse.setStatusResponse(status);
	} else if (response == "PREAUTH" && !tagged) {
		qDebug("IMAPResponseParser: responseParser: got untagged PREAUTH response.");
		// XXX
	} else if (response == "BYE") {
		IMAPResponseStatusResponse status(BYE, line);
		if (!tagged) status.setExitedUnexpected(true);
		_iresponse.setStatusResponse(status);
	} else if (response == "CAPABILITY") {
		IMAPResponseCAPABILITY capability(QStringList::split(' ', line));
		_iresponse.addCAPABILITY(capability);
	} else if (response == "LIST") {
		QStringList list = splitData(line, true);

		QStringList flags;
		parseParenthesizedList(list[0], flags);

		removeLimiters(list[1]);
		removeLimiters(list[2]);		
		IMAPResponseLIST rlist(parseFlagList(flags), list[1], list[2]);
		_iresponse.addLIST(rlist);
	} else if (response == "LSUB") {
		QStringList list = splitData(line, true);

		QStringList flags;
		parseParenthesizedList(list[0], flags);

		removeLimiters(list[1]);
		removeLimiters(list[2]);
		IMAPResponseLSUB lsub(parseFlagList(flags), list[1], list[2]);
		_iresponse.addLSUB(lsub);
	} else if (response == "STATUS") {
		QStringList list = splitData(line, true);

		removeLimiters(list[0]);
		IMAPResponseSTATUS status(list[0]);

		QStringList flags;
		parseParenthesizedList(list[1], flags);
		QStringList::Iterator it;
		for (it = flags.begin(); it != flags.end(); it++) {
			if (*it == "MESSAGES") status.setMessages(*(++it));
			else if (*it == "RECENT") status.setRecent(*(++it));
			else if (*it == "UIDNEXT") status.setUidnext(*(++it));
			else if (*it == "UIDVALIDITY") status.setUidvalidity(*(++it));
			else if (*it == "UNSEEN") status.setUnseen(*(++it));
			else qWarning((QString("IMAPResponseParser: parseResponse: Unknown status data: " )+ *(it++) + "|").latin1());
		}
		_iresponse.addSTATUS(status);
	} else if (response == "SEARCH") {
		IMAPResponseSEARCH search(QStringList::split(' ', line));
		_iresponse.addSEARCH(search);
	} else if (response == "FLAGS") {
		QStringList list;
		parseParenthesizedList(line, list);

		IMAPResponseFLAGS flags(parseFlagList(list));
		_iresponse.addFLAGS(flags);
	} else if (isNum) {
		QStringList list = QStringList::split(' ', line);
		if (list[0] == "EXISTS") {
			IMAPResponseEXISTS exists(response);
			_iresponse.addEXISTS(exists);
		} else if (list[0] == "RECENT") {
			IMAPResponseRECENT recent(response);
			_iresponse.addRECENT(recent);
		} else if (list[0] == "EXPUNGE") {
			IMAPResponseEXPUNGE expunge(response);
			_iresponse.addEXPUNGE(expunge);
		} else if (list[0] == "FETCH") {
			IMAPResponseFETCH fetch;
			QStringList::Iterator it;

			qDebug ( "Got FETCH\n" );

			QStringList fetchList = splitData(line, true);
			QStringList list;
			
			qDebug ( "fl [0]=%s, fl [1]=%s, fl[2]=%s\n", fetchList[0].latin1(),fetchList[1].latin1(),fetchList[2].latin1());
			
			parseParenthesizedList(fetchList[1], list);

			for (it = list.begin(); it != list.end(); it++) {
				qDebug ( "Checking list[] == %s\n", (*it).latin1());
			
				if (*it == "BODY") {
					qDebug("IMAPResponseParser: responseParser: got FETCH::BODY");
					// XXX
				} else if ((*it).find(QRegExp("^BODY\\[\\d+\\]")) != -1) {
					qDebug("IMAPResponseParser: responseParser: got FETCH::BODY[x]");

					QString number = ( *it ). mid ( 5, ( *it ). length ( ) - 6 );
					QString bodydata = *(++it);

//					QStringList blist;
//					parseParenthesizedList(bodydata, blist);

					IMAPResponseBodyPart bodypart;
//					QString tmp;
//					for (unsigned int i = 2; i < blist.count(); i++) {
//						if (i != 2) tmp += " " + blist[i];
//						else tmp += blist[i];
//					}
					bodypart.setData(bodydata);

//					QString tmp = list[0];
//					tmp.replace(0, 5, "");
//					tmp.truncate(blist[0].length() - 1);
					bodypart.setPart(number);

					qDebug("added bodypart [%s]: '%s'\n\n", number.latin1(), bodydata.latin1());

					fetch.addBodyPart(bodypart);
				} else if (*it == "BODYSTRUCTURE") {
					qDebug("IMAPResponseParser: responseParser: got FETCH::BODYSTRUCTURE");
/*
					QString bsdata = *(++it);
					QStringList bsList;
					parseParenthesizedList(bsdata, bsList);

					IMAPResponseBodystructure bodystructure;
					QStringList attachml;

					for (int i = 0; i < bsList.count() - 1; i++) {
						parseParenthesizedList(bsList[0], attachml);

						IMAPResponseBodypart bodypart;
						bodypart.setMimeTypeMain(attachml[0] == "NIL" ? QString(0) : attachml[0]);
						bodypart.setMimeTypeSub(attachml[1] == "NIL" ? QString(0) : attachml[1]);
						bodypart.setAddData(attachml[2] == "NIL" ? QString(0) : attachml[2]);
						// 3 (NIL)
						// 4 (NIL)
						bodypart.setEncoding(attachml[5] == "NIL" ? QString(0) : attachml[5]);
						bodypart.setSize(attachml[6] == "NIL" ? QString(0) : attachml[6]);
						bodypart.setLength(attachml[7] == "NIL" ? QString(0) : attachml[7]);
						bodypart.setAttachInfo(attachml[8] == "NIL" ? QString(0) : attachml[8]);
						// 9 (NIL)
						// 10 (NIL)

						bodystructure.addBodyPart(bodypart);
					}
*/
				} else if (*it == "ENVELOPE") {
					QString envdata = *(++it);
					QStringList envList;
					parseParenthesizedList(envdata, envList);

					IMAPResponseEnvelope envelope;
					envelope.setMailDate(envList[0] == "NIL" ? QString(0) : removeLimiters(envList[0]));
					envelope.setSubject(envList[1] == "NIL" ? QString(0) : removeLimiters(envList[1]));

					QStringList froml, senderl, replytol, tol, ccl, bccl;
					QStringList froma, sendera, replytoa, toa, cca, bcca;
					parseParenthesizedList(envList[2], froml);
					parseParenthesizedList(envList[3], senderl);
					parseParenthesizedList(envList[4], replytol);
					parseParenthesizedList(envList[5], tol);
					parseParenthesizedList(envList[6], ccl);
					parseParenthesizedList(envList[7], bccl);

					QStringList::Iterator it;
					for (it = froml.begin(); it != froml.end(); it++) {
						parseParenthesizedList(*it, froma);
						if (froml[0] != "NIL")
							envelope.addFrom(IMAPResponseAddress(
								removeLimiters(froma[0]),
								removeLimiters(froma[1]),
								removeLimiters(froma[2]),
								removeLimiters(froma[3])));
					}

					for (it = senderl.begin(); it != senderl.end(); it++) {
						parseParenthesizedList(*it, sendera);
						if (senderl[0] != "NIL")
							envelope.addSender(IMAPResponseAddress(
								removeLimiters(sendera[0]), 
								removeLimiters(sendera[1]), 
								removeLimiters(sendera[2]), 
								removeLimiters(sendera[3])));
					}

					for (it = replytol.begin(); it != replytol.end(); it++) {
						parseParenthesizedList(*it, replytoa);				
						if (replytol[0] != "NIL")
							envelope.addReplyTo(IMAPResponseAddress(
								removeLimiters(replytoa[0]),
								removeLimiters(replytoa[1]),
								removeLimiters(replytoa[2]), 
								removeLimiters(replytoa[3])));
					}

					for (it = tol.begin(); it != tol.end(); it++) {
						parseParenthesizedList(*it, toa);
						if (tol[0] != "NIL") 
							envelope.addTo(IMAPResponseAddress(
								removeLimiters(toa[0]),
								removeLimiters(toa[1]),
								removeLimiters(toa[2]),
								removeLimiters(toa[3])));
					}

					for (it = ccl.begin(); it != ccl.end(); it++) {
						parseParenthesizedList(*it, cca);
						if (ccl[0] != "NIL")
							envelope.addCc(IMAPResponseAddress(
								removeLimiters(cca[0]),
								removeLimiters(cca[1]),
								removeLimiters(cca[2]),
								removeLimiters(cca[3])));
					}

					for (it = bccl.begin(); it != bccl.end(); it++) {
						parseParenthesizedList(*it, bcca);
						if (bccl[0] != "NIL")
							envelope.addBcc(IMAPResponseAddress(
								removeLimiters(bcca[0]),
								removeLimiters(bcca[1]),
								removeLimiters(bcca[2]),
								removeLimiters(bcca[3])));
					}

					envelope.setInReplyTo(envList[7] == "NIL" ? QString(0) : removeLimiters(envList[7]));
					envelope.setMessageId(envList[8] == "NIL" ? QString(0) : removeLimiters(envList[8]));
				
					fetch.setEnvelope(envelope);
				} else if (*it == "FLAGS") {
					QString flagdata = *(++it);
					QStringList flags;
					parseParenthesizedList(flagdata, flags);
					fetch.setFlags(parseFlagList(flags));
				} else if (*it == "INTERNALDATE") {
					fetch.setInternalDate(removeLimiters(*(++it)));
				} else if (*it == "RFC822" || *it == "BODY[]") {
					qDebug("IMAPResponseParser: responseParser: got FETCH::RFC822");
					// XXX
				} else if (*it == "RFC822.HEADER" || *it == "BODY.PEEK[HEADER]") {
					qDebug("IMAPResponseParser: responseParser: got FETCH::RFC822.HEADER");
					// XXX
				} else if (*it == "RFC822.SIZE") {
					fetch.setRFC822Size(*(++it));
				} else if (*it == "RFC822.TEXT" || *it == "BODY[TEXT]") {
					qDebug("IMAPResponseParser: responseParser: got FETCH::RFC822.TEXT");
					// XXX
				} else if (*it == "UID") {
					fetch.setUid(*(++it));
				}	
			}
			_iresponse.addFETCH(fetch);
		}
	} else qWarning((QString("IMAPResponseParser: parseResponse: Unknown response: ") + response + "|").latin1());
	
}

QStringList IMAPResponseParser::splitData(const QString &data, bool withBrackets)
{
	int b = 0;
	bool a = false, noappend = false, escaped = false;	 
	QString temp;
	QStringList list;

	qDebug ( "sd: '%s'\n", data.latin1());

	for (unsigned int i = 0; i <= data.length(); i++) {
		if (withBrackets && data[i] == '(' && !a) b++;
		else if (withBrackets && data[i] == ')' && !a) b--;

		if (data[i] == '{' && !escaped && !a ) {
			qDebug ( "sd: found a {\n" );
		
			int p = data. find ( '}', i + 1 );
			int eol = data. find ( '\n', i + 1 );

			if ( p > int( i )) {
				int len = data. mid ( i + 1, p - i - 1 ). toInt ( );
		
				qDebug ( "sd: skipping %d bytes\n", len );
			
				if ( b == 0 ) {			
					temp = data. mid ( eol + 1, len );
					noappend = false;
					i = eol + len;
					continue;
				}
				else {
					temp. append ( '{' );
					temp. append ( QString::number ( len ));
					temp. append ( "}\r\n" );
					temp. append ( data. mid ( eol + 1, len ));
					i = eol + len;
					continue;
				}
			}
		}

		if (data[i] == '\"' && !escaped) a = !a;
		else escaped = false;

		if (data[i] == '\\' && data[i + 1] == '\"') escaped = true;

		if ((data[i] == ' ' || i == data.length()) && b == 0 && !a) {
			list.append(temp);
			temp = QString::null;
			if (data[i] == ' ') noappend = true;
		}

		if (!noappend) temp += data[i];
		noappend = false;
	}

	return list;
}

void IMAPResponseParser::parseParenthesizedList(const QString &data, QStringList &parsed)
{
	QString data_(data);
	removeLimiters(data_, '(', ')');
	parsed = splitData(data_, true);
}

void IMAPResponseParser::splitTagData(const QString &line, QString &tag, QString &data)
{
	int pos;
	if ((pos = line.find(' ')) != -1) {
		tag = line.left(pos);
		data = line.right(line.length() - pos - 1);
	} else qWarning((QString("IMAPResponseParser: splitTagData: tag not found. Line was ") + line + "|").latin1());
}

QString IMAPResponseParser::removeLimiters(QString &string, const QChar &sl, const QChar &el)
{
	QString tmpString;
	string = string. stripWhiteSpace ( );
	
	if (string[0] == sl && string[string.length() - 1] == el) {
		string.truncate(string.length() - 1);
		string.replace(0, 1, "");

		for (unsigned int i = 1; i <= string.length(); i++) {
			if (string[i - 1] == '\\' && sl == '\"') ++i;
			tmpString += string[i - 1];
		}
	}
	return tmpString;
}

IMAPResponseEnums::IMAPResponseCode IMAPResponseParser::getResponseCode(const QString &line)
{
	if (line.find(QRegExp((QString) "^\\[.*\\]" + ' ' + ".*")) != -1) {
		int pos = line.find("] ");
		QString code = line.left(pos + 1).upper();

		if (code.find(QRegExp("[ALERT]")) != -1) return ALERT;
		else if (code.find(QRegExp("[NEWNAME .* .*]")) != -1) return NEWNAME; // XXX
		else if (code.find(QRegExp("[PARSE]")) != -1) return PARSE;
		else if (code.find(QRegExp("[PERMANENTFLAGS \\d*]")) != -1) return PERMANENTFLAGS; // XXX
		else if (code.find(QRegExp("[READ-ONLY]")) != -1) return READONLY;
		else if (code.find(QRegExp("[READ-WRITE]")) != -1) return READWRITE;
		else if (code.find(QRegExp("[TRYCREATE]")) != -1) return TRYCREATE;
		else if (code.find(QRegExp("[UIDVALIDITY \\d*]")) != -1) return UIDVALIDITY; // XXX
		else if (code.find(QRegExp("[UNSEEN \\d*]")) != -1) return UNSEEN; // XXX
		else {
			qWarning((QString("IMAPResponseParser: getResponseCode: Unknown code: ") + code + "|").latin1());
			return UnknownCode;
		}
	}
	return NoCode;
}

QValueList<IMAPResponseEnums::IMAPResponseFlags> IMAPResponseParser::parseFlagList(const QStringList &flagList)
{
	QValueList<IMAPResponseFlags> flags;
	QStringList::ConstIterator it;
	for (it = flagList.begin(); it != flagList.end(); it++) {
		QString flag = (*it).lower();
		if (flag == "\\seen") flags.append(Seen);
		else if (flag == "\\answered") flags.append(Answered);
		else if (flag == "\\flagged") flags.append(Flagged);
		else if (flag == "\\deleted") flags.append(Deleted);
		else if (flag == "\\draft") flags.append(Draft);
		else if (flag == "\\recent") flags.append(Recent);
		else if (flag == "\\noinferiors") flags.append(Noinferiors);
		else if (flag == "\\noselect") flags.append(Noselect);
		else if (flag == "\\marked") flags.append(Marked);
		else if (flag == "\\unmarked") flags.append(Unmarked);
		else if (flag.isEmpty()) { }
		else qWarning((QString("IMAPResponseParser: parseFlagList: Unknown flag: ") + *it + "|").latin1());
	}
	return flags;
}

