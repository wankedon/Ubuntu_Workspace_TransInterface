#pragma once
#include <boost/asio.hpp> 
using namespace boost;

using PackageToSend = std::pair<asio::ip::udp::endpoint, std::vector<char>>;
using ReceiveHandler = std::function<void(asio::ip::udp::endpoint, asio::const_buffer)>;

class RepeatedTimer;
class UDPTransport
{
public:
	UDPTransport(short serverPort, const std::string address, ReceiveHandler handler);
	~UDPTransport();

public:
	bool start();
	void stop();
	void startTimer(const uint64_t timeout_ms, std::function<void()> timeout_handler);
	void send(const asio::const_buffer& buffer);
	void sendBuffer(const asio::ip::udp::endpoint& receiver, const asio::const_buffer& buffer);

private:
	void doReceive();
	void onSendComplete(std::error_code ec, std::size_t bytes_sent);

private:
	asio::io_context m_context;
	asio::ip::udp::socket m_socket;
	asio::ip::udp::endpoint m_sender_endpoint;
	std::vector<char> m_receiveBuf;
	std::list<PackageToSend> m_packagesToSend;
	ReceiveHandler m_dataHandler;
	std::future<void> m_future;
	std::shared_ptr<RepeatedTimer> m_timer;
	std::mutex m_mutex;
};