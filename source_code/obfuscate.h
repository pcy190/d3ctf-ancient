#ifndef ACIENT_OBFUSCATE_H
#define ACIENT_OBFUSCATE_H

#include <cstddef>

namespace ay
{
    // Obfuscates a string at compile time
    template <std::size_t N, char KEY>
    class Obfuscator
    {
    public:
        // Obfuscates the string 'data' on construction
        constexpr Obfuscator(const char* data)
        {
            static_assert(KEY != '\0', "KEY must not be the null character.");

            // On construction each of the characters in the string is
            // obfuscated with an XOR cipher based on KEY
            for (std::size_t i = 0; i < N; i++)
            {
                m_data[i] = data[i] ^ KEY;
            }
        }

        constexpr const char* getData() const
        {
            return &m_data[0];
        }

        constexpr std::size_t getSize() const
        {
            return N;
        }

        constexpr char getKey() const
        {
            return KEY;
        }

    private:

        char m_data[N]{};
    };

    // Handles decryption and re-encryption of an encrypted string at runtime
    template <std::size_t N, char KEY>
    class obfuscated_data
    {
    public:
        obfuscated_data(const Obfuscator<N, KEY>& obfuscator)
        {
            for (std::size_t i = 0; i < N; i++)
            {
                m_data[i] = obfuscator.getData()[i];
            }
        }

        ~obfuscated_data()
        {
            // Zero m_data to remove it from memory
            for (std::size_t i = 0; i < N; i++)
            {
                m_data[i] = 0;
            }
        }

        // Returns a pointer to the plain text string, decrypting it if
        // necessary
        operator char*()
        {
            decrypt();
            return m_data;
        }

        // Manually decrypt the string
        void decrypt()
        {
            if (is_encrypted())
            {
                for (std::size_t i = 0; i < N; i++)
                {
                    m_data[i] ^= KEY;
                }
            }
        }

        // Manually re-encrypt the string
        void encrypt()
        {
            if (!is_encrypted())
            {
                for (std::size_t i = 0; i < N; i++)
                {
                    m_data[i] ^= KEY;
                }
            }
        }

        // Returns true if this string is currently encrypted, false otherwise.
        bool is_encrypted() const
        {
            return m_data[N - 1] != '\0';
        }

    private:

        // Local storage for the string. Call is_encrypted() to check whether or
        // not the string is currently obfuscated.
        char m_data[N];
    };

    // This function exists purely to extract the number of elements 'N' in the
    // array 'data'
    template <std::size_t N, char KEY = '.'>
    constexpr auto make_obfuscator(const char(&data)[N])
    {
        return Obfuscator<N, KEY>(data);
    }
}

// Obfuscates the string 'data' at compile-time and returns a reference to a
// ay::obfuscated_data object with global lifetime that has functions for
// decrypting the string and is also implicitly convertable to a char*
#define AY_OBFUSCATE(data) AY_OBFUSCATE_KEY(data, '.')

// Obfuscates the string 'data' with 'key' at compile-time and returns a
// reference to a ay::obfuscated_data object with global lifetime that has
// functions for decrypting the string and is also implicitly convertable to a
// char*
#define AY_OBFUSCATE_KEY(data, key) \
	[]() -> ay::obfuscated_data<sizeof(data)/sizeof(data[0]), key>& { \
		constexpr auto n = sizeof(data)/sizeof(data[0]); \
		static_assert(data[n - 1] == '\0', "String must be null terminated"); \
		constexpr auto obfuscator = ay::make_obfuscator<n, key>(data); \
		static auto obfuscated_data = ay::obfuscated_data<n, key>(obfuscator); \
		return obfuscated_data; \
	}()

#endif //ACIENT_OBFUSCATE_H
