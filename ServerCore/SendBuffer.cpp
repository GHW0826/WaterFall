#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 allocSize)
	: _owner(owner),
	_buffer(buffer),
	_allocSize(allocSize)
{}

SendBuffer::~SendBuffer() {
}

/*
void SendBuffer::CopyData(void* data, int32 len)
{
	ASSERT_CRASH(Capacity() >= len);
	::memcpy(_buffer.data(), data, len);
	_writeSize = len;
}
*/

void SendBuffer::Close(uint32 writeSize) {
	ASSERT_CRASH(_allocSize >= writeSize);
	_writeSize = writeSize;
	_owner->Close(writeSize);
}

// 버퍼 청크는 TLS 공간을 사용하기 때문에 싱글 스레드라고 생각하면 편함
SendBufferChunk::SendBufferChunk() {
}

SendBufferChunk::~SendBufferChunk() {
}

void SendBufferChunk::Reset() {
	_open = false;
	_usedSize = 0;
}

SendBufferRef SendBufferChunk::Open(uint32 allocSize) {
	ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
	ASSERT_CRASH(_open == false);

	if (allocSize > FreeSize())
		return nullptr;

	_open = true;
	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), allocSize);
}

void SendBufferChunk::Close(uint32 writeSize) {
	ASSERT_CRASH(_open == true);
	_open = false;
	_usedSize += writeSize;
}



SendBufferRef SendBufferManager::Open(uint32 size) {

	if (LSendBufferChunk == nullptr) {
		LSendBufferChunk = Pop(); // WRITE_LOCK
		LSendBufferChunk->Reset();
	}

	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

	// 다 썻으면 버리고 새거로 교체
	if (LSendBufferChunk->FreeSize() < size) {
		LSendBufferChunk = Pop(); // WRITE_LOCK;
		LSendBufferChunk->Reset();
	}

	return LSendBufferChunk->Open(size);
}

SendBufferChunkRef SendBufferManager::Pop() {
	// sendBufferChunk를 다 소진하면 반납
	{
		WRITE_LOCK;
		if (_sendBufferChunks.empty() == false) {
			SendBufferChunkRef sendBufferChunk = _sendBufferChunks.back();
			_sendBufferChunks.pop_back();
			return sendBufferChunk;
		}
	}

	// SendBufferChunkRef는 레퍼 카운트가 0이 되어서 사용하지 않으면
	// 메모리를 날리는게 아니라 PushGlobal로 들어가 SendbufferChunk모임에 반환
	return SendBufferChunkRef(xnew<SendBufferChunk>(), PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef buffer) {

	WRITE_LOCK;
	_sendBufferChunks.push_back(buffer);

}

void SendBufferManager::PushGlobal(SendBufferChunk* buffer) {
	GSendBufferManager->Push(SendBufferChunkRef(buffer, PushGlobal));
}


