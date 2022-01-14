FROM debian:latest as builder

ENV DEBIAN_FRONTEND noninteractive
ENV LANG=C.UTF-8

RUN apt update -y
RUN apt upgrade -y

RUN apt install -y git gcc make libssl-dev dh-autoreconf

WORKDIR /tmp
RUN git clone https://github.com/lundman/lundftpd.git
WORKDIR /tmp/lundftpd
RUN git clone https://github.com/lundman/lion.git
RUN autoreconf --install
RUN ./configure
RUN make

WORKDIR /tmp
RUN openssl req -newkey rsa:2048 -new -nodes -x509 -days 3650 -keyout key.pem -out cert.pem -batch
RUN cat key.pem cert.pem > lundftpd.pem

# ---

FROM debian:latest

ENV DEBIAN_FRONTEND noninteractive
ENV LANG=C.UTF-8

RUN ln -fs /usr/share/zoneinfo/Europe/Vienna /etc/localtime
RUN dpkg-reconfigure -f noninteractive tzdata
RUN echo "LANG=C.UTF-8" > /etc/default/locale

RUN addgroup --gid 1000 lundftpd
RUN adduser --uid 1000 --gid 1000 --shell /usr/sbin/nologin --disabled-password --gecos "" lundftpd

WORKDIR /home/lundftpd
RUN mkdir ftp-data
RUN mkdir ftp-logs
RUN mkdir files

COPY --from=builder /tmp/lundftpd/src/lundftpd /home/lundftpd/lundftpd
COPY --from=builder /tmp/lundftpd/lundftpd.conf /home/lundftpd/ftp-data/lundftpd.conf
COPY --from=builder /tmp/lundftpd.pem /home/lundftpd/ftp-data/lundftpd.pem

RUN chown -R lundftpd /home/lundftpd
RUN chgrp -R lundftpd /home/lundftpd

USER lundftpd

VOLUME ["/home/lundftpd/ftp-data","/home/lundftpd/ftp-logs","/home/lundftpd/files"]

EXPOSE 6688 6689-6699

CMD ["/home/lundftpd/lundftpd", "-v", "-f", "/home/lundftpd/ftp-data/lundftpd.conf"]
