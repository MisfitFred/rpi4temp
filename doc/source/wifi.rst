WIFI module
===========

This software module takes care of the WIFI connection. It also supervises it and tries to reconnect if the connection is lost.


Connection state machine 
------------------------



.. uml::
    
    (IDLE) --> (CONNECTING): Linkstatus[down]/startConnection
    (IDLE) --> (FAILURE): wrong connection state
    (CONNECTING) --> (CONNECTING): timeout[not reached] /
    (CONNECTING) --> (CONNECTING): Linkstatus[DOWN | JOIN | NOIP] /
    (CONNECTING) --> (FAILURE): (Linkstatus[FAIL | NONET ] or timeout[reached]) AND badAuthRetries < 3
    (CONNECTING) --> (FAILURE): Linkstatus[BADAUTH] / badAuthRetries++
    (CONNECTING) --> (CONNECTED): Linkstatus[UP]  / badAuthRetries=0
    (CONNECTED) --> (FAILURE): Linkstatus[ not UP]
    (FAILURE) --> (CONNECTING) : wait time elapsed
    (FAILURE) --> (BADAUTH): badAuthRetries >= 3


