# This is a dummy test device to verify shared memory memmapped files capabilities
FROM centos:7
LABEL Author="Michal Karm Babacek <karm@email.cz>"
RUN yum install unzip -y && yum clean all && rm -rf /var/cache/yum /tmp/* && \
    useradd -s /sbin/nologin ipranger && mkdir /opt/iprangerzip && chown ipranger /opt/iprangerzip
USER    ipranger
WORKDIR /opt/
ENV MODE TBD
VOLUME /opt/iprangerzip/testdb
COPY --chown=ipranger:ipranger container/start.sh container/iprangerzip.zip /opt/
RUN unzip iprangerzip.zip
CMD ["/opt/start.sh"]
