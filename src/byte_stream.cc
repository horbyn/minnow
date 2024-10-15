#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity )
  : capacity_ { capacity }
  , error_ { false }
  , buffer_ {}
  , buffv_ {}
  , closed_ { false }
  , pushed_ { 0 }
  , stash_ { 0 }
  , popped_ { 0 }
{}

bool Writer::is_closed() const
{
  return ByteStream::closed_;
}

void Writer::push( string data )
{
  uint64_t avail = this->available_capacity();
  if ( this->is_closed() || avail == 0 ) {
    return;
  }

  uint64_t to_push = std::min( avail, data.size() );
  if ( to_push == 0 ) {
    return;
  }

  if ( ByteStream::stash_ > 0 ) {
    ByteStream::buffer_.emplace_back( std::string { ByteStream::buffv_ } + data );
    ByteStream::buffer_.pop_front();
  } else {
    ByteStream::buffer_.emplace_back( data );
  }

  ByteStream::buffv_ = std::string_view { ByteStream::buffer_.back().c_str(), ByteStream::stash_ + to_push };
  ByteStream::pushed_ += to_push;
  ByteStream::stash_
    = ByteStream::stash_ + to_push < ByteStream::capacity_ ? ByteStream::stash_ + to_push : ByteStream::capacity_;
}

void Writer::close()
{
  ByteStream::closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  return ByteStream::capacity_ > ByteStream::stash_ ? ByteStream::capacity_ - ByteStream::stash_ : 0;
}

uint64_t Writer::bytes_pushed() const
{
  return ByteStream::pushed_;
}

bool Reader::is_finished() const
{
  return ByteStream::writer().is_closed() && ( this->bytes_buffered() == 0 );
}

uint64_t Reader::bytes_popped() const
{
  return ByteStream::popped_;
}

string_view Reader::peek() const
{
  return ByteStream::buffv_;
}

void Reader::pop( uint64_t len )
{
  if ( this->is_finished() ) {
    return;
  }

  uint64_t to_pop = std::min( len, ByteStream::stash_ );
  if ( to_pop == 0 ) {
    return;
  }

  if ( to_pop < ByteStream::capacity_ && to_pop < ByteStream::buffer_.front().size() ) {
    ByteStream::buffv_.remove_prefix( to_pop );
  } else {
    ByteStream::buffer_.pop_front();
    if ( ByteStream::buffer_.empty() ) {
      ByteStream::buffv_ = {};
    }

  } // end if-else()

  ByteStream::popped_ += to_pop;
  ByteStream::stash_ = ByteStream::stash_ > to_pop ? ByteStream::stash_ - to_pop : 0;
}

uint64_t Reader::bytes_buffered() const
{
  return ByteStream::stash_;
}
