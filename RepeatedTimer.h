#pragma once
#include <memory>
#include <boost/asio.hpp> 
using namespace boost;

class RepeatedTimer : public std::enable_shared_from_this<RepeatedTimer>
{
public:
	RepeatedTimer(asio::io_service& io_service,
		std::function<void()> timeout_handler)
		: io_service_(io_service), timer_(io_service_),
		timeout_handler_(std::move(timeout_handler)) {}

	void Start(const uint64_t timeout_ms) { Reset(timeout_ms); }

	void Stop() { is_running_.store(false); }

	void Reset(const uint64_t timeout_ms) {
		is_running_.store(true);
		DoSetExpired(timeout_ms);

	}

private:
	void DoSetExpired(const uint64_t timeout_ms) {
		if (!is_running_.load()) { return; }
		auto self(shared_from_this());
		timer_.expires_from_now(std::chrono::milliseconds(timeout_ms));
		timer_.async_wait([self, timeout_ms](const boost::system::error_code& e) {
			if (e.value() == asio::error::operation_aborted || !self->is_running_.load()) { return; }
			self->timeout_handler_();
			self->DoSetExpired(timeout_ms);
			});
	}

private:
	// The io service that runs this timer.
	asio::io_service& io_service_;
	// The actual boost timer.
	asio::steady_timer timer_;

	std::atomic<bool> is_running_ = { false };
	// The handler that will be triggered once the time's up.
	std::function<void()> timeout_handler_;
};