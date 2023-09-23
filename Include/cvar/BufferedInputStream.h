// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: BufferedInputStream.h - buffered input stream class header
// author: Karl-Mihkel Ott

#pragma once

#include <istream>
#include <ostream>
#include <cvar/Api.h>

#define DEFAULT_BUFFER 8192

namespace cvar {

	template <size_t N = DEFAULT_BUFFER>
	class CVAR_API BufferedInputStream {
		private:
			std::istream& m_stream;
			char m_arrData[N]{};

			size_t m_uStreamLen = 0;
			size_t m_uStreamDataAvail = 0;
			size_t m_uBufDataAvail = 0;
			size_t m_uBufCounter = 0;

		private:
			bool _ReadBuf() {
				if (!m_uStreamDataAvail)
					return false;

				// check if the last character should be the first one
				if (m_uBufCounter && m_uBufDataAvail - m_uBufCounter <= 1) {
					m_arrData[0] = m_arrData[m_uBufDataAvail - 1];
					if (m_uStreamDataAvail >= N-1) {
						m_stream.read(m_arrData + 1, N - 1);
						m_uStreamDataAvail -= (N - 1);
						m_uBufDataAvail = N;
					}
					else {
						m_stream.read(m_arrData + 1, m_uStreamDataAvail);
						m_uBufDataAvail = m_uStreamDataAvail;
						m_uStreamDataAvail = 0;
					}
				}
				else {
					if (m_uStreamDataAvail >= N) {
						m_stream.read(m_arrData, N);
						m_uStreamDataAvail -= N;
						m_uBufDataAvail = N;
					}
					else {
						m_stream.read(m_arrData, m_uStreamDataAvail);
						m_uBufDataAvail = m_uStreamDataAvail;
						m_uStreamDataAvail = 0;
					}
				}

				m_uBufCounter = 0;

				return true;
			}

		public:
			BufferedInputStream(std::istream& _stream) :
				m_stream(_stream) 
			{
				m_stream.seekg(0, std::ios_base::end);
				m_uStreamLen = m_stream.tellg();
				m_stream.seekg(0, std::ios_base::beg);
				m_uStreamDataAvail = m_uStreamLen;

				_ReadBuf();
			}

			inline std::istream& raw() {
				return m_stream;
			}

			inline char peek() {
				if (m_uBufCounter >= m_uBufDataAvail - 1) {
					if (!_ReadBuf())
						return -1;
				}

				return m_arrData[m_uBufCounter];
			}

			inline char get() {
				if (m_uBufCounter >= m_uBufDataAvail) {
					if (!_ReadBuf())
						return -1;
				}

				return m_arrData[m_uBufCounter++];
			}

			inline bool eof() {
				return !m_uStreamDataAvail;
			}
	};
}
