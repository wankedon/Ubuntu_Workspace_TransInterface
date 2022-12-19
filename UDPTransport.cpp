#include"stdafx.h"
#include "UDPTransport.h"
#include "RepeatedTimer.h"
using namespace std;

UDPTransport::UDPTransport(short serverPort, const std::string address, ReceiveHandler handler)
	:m_socket(m_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), serverPort)),
	m_receiveBuf(1024),
	m_dataHandler(handler)
{
	auto size = address.find(":");
	auto ip = address.substr(0, size);
	short port = atoi(address.substr(size + 1, 4).c_str());
	m_sender_endpoint= asio::ip::udp::endpoint(asio::ip::address::from_string(ip), port);
}

UDPTransport::~UDPTransport()
{
	stop();
}

void UDPTransport::startTimer(const uint64_t timeout_ms, std::function<void()> timeout_handler)
{
	m_timer = make_shared<RepeatedTimer>(m_context, timeout_handler);
	m_timer->Start(timeout_ms);
}

void UDPTransport::doReceive()
{
	auto handler = [this](std::error_code ec, std::size_t bytes_recvd)
	{
		if (!ec && bytes_recvd > 0)
		{
			const auto* p = m_receiveBuf.data();
			m_dataHandler(m_sender_endpoint, asio::buffer(p, bytes_recvd));
		}
		doReceive();
	};
	m_socket.async_receive_from(asio::buffer(m_receiveBuf), m_sender_endpoint, handler);
}

void UDPTransport::send(const asio::const_buffer& buffer)
{
	sendBuffer(m_sender_endpoint, buffer);
}

void UDPTransport::sendBuffer(const asio::ip::udp::endpoint& receiver, const asio::const_buffer& buffer)
{
	auto* p = (const char*)buffer.data();
	std::vector<char> buf{ p, p + buffer.size() };
	std::lock_guard<mutex> lg(m_mutex);
	bool isPending = !m_packagesToSend.empty();
	m_packagesToSend.emplace_back(receiver, std::move(buf));
	if (!isPending)
	{
		m_socket.async_send_to(
			asio::buffer(m_packagesToSend.front().second), receiver,
			[this](std::error_code ec, std::size_t bytes_sent)
			{
				onSendComplete(ec, bytes_sent);
			});
	}
}

void UDPTransport::onSendComplete(std::error_code ec, std::size_t bytes_sent)
{
	//fmt::print("send complete ec = {}, send count = {}\n", ec.value(), bytes_sent);
	std::lock_guard<mutex> lg(m_mutex);
	assert(!m_packagesToSend.empty());
	m_packagesToSend.pop_front();
	//fmt::print("pending buffers = {}\n", m_packagesToSend.size());
	if (!m_packagesToSend.empty())
	{
		auto& head = m_packagesToSend.front();
		m_socket.async_send_to(
			asio::buffer(head.second), head.first,
			[this](std::error_code ec, std::size_t bytes_sent)
			{
				onSendComplete(ec, bytes_sent);
			});
	}
}

bool UDPTransport::start()
{
	if (m_future.valid())
		return false;
	m_future = std::async([this] {doReceive(); m_context.run(); });
	return true;
}

void UDPTransport::stop()
{
	if (m_timer)
	{
		m_timer->Stop();
	}
	if (!m_future.valid())
		return;
	m_context.stop();
	m_future.get();
}

namespace boost
{
	void throw_exception(std::exception const& e)
	{
		return;
	}
}