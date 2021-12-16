
This is the latest ircbot to go with LundFTPD v3.x and LiON. It uses
LiON and talks version 2 of the ircbot protocol. It obsoletes "herald"
and "fantti".

It needs a configuration file, where you define things like:

 udp-listen-definition1
   +port <int>
   +password <string>

   +irc-definition1
     +host <str>
     +port <int>
     +[password <str>]
     +nick <str>
     +name <str>
     
     +channel-definition1
       +channel <str>
       +[key <str>]

     . (more channels)
   . (more irc servers)
 .

 udp-listen-definition2
  etc

So, for each listening udp port, with password, you should be able to
list irc-hosts you want it to connect to, and for each irc-host you
list, you should be able to list channels for it to join.

Currently, there is only logic for ONE channel per irc host, but we
will eventually sort that out.



So, udp node hold information like lion_t handle to udp listening
socket, the BF_KEY to decrypt the input data (and encrypt if you send
presumably). It also contains stored information, things like "cps"
and number of users online etc.

irc node stores all data for a irc server, and a pointer to its udp
node associated with it.


      +------------+       +------------+
      | UDP defn 1 |       | UDP defn 2 |
      +------------+       +------------+
             |
             |
      +------------+
      | Irc Server |
      | & nick     |
      +------------+
             |
