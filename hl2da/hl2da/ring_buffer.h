
#include <vector>

template <typename T>
class ring_buffer
{
private:
    struct search_interval
    {
        int32_t l;
        int32_t r;
    };

    int32_t m_size;

    std::vector<T> m_frames;
    std::vector<uint64_t> m_timestamps;

    int32_t m_write;
    int32_t m_count;

    search_interval search(uint64_t timestamp, int32_t l, int32_t r, int32_t size) const
    {
        while ((r - l) > 1)
        {
            int32_t m = (r + l) / 2;
            int32_t m_index = m % size;
            uint64_t t = m_timestamps[m_index];
            if (t > timestamp) { r = m; } else if (t < timestamp) { l = m; } else { return { m_index, m_index }; }
        }

        return { l % size, r % size };
    }

    int32_t find_index(uint64_t timestamp, int time_preference, bool tiebreak_right, int32_t size) const
    {
        if (size <= 0) { return -1; }

        search_interval si = search(timestamp, m_write, m_write + size - 1, size);

        if (si.l == si.r) { return si.l; }

        uint64_t tl = m_timestamps[si.l];
        uint64_t tr = m_timestamps[si.r];

        if (timestamp <= tl) { return si.l; }
        if (timestamp >= tr) { return si.r; }

        if (time_preference < 0) { return si.l; }
        if (time_preference > 0) { return si.r; }

        uint64_t dl = timestamp - tl;
        uint64_t dr = tr - timestamp;

        if (dl < dr) { return si.l; }
        if (dl > dr) { return si.r; }

        return tiebreak_right ? si.r : si.l;
    }

public:
    ring_buffer(int32_t size)
    {
        reset(size);
    }

    ring_buffer() : ring_buffer(16)
    {

    }

    void reset()
    {
        m_frames.clear();
        m_timestamps.clear();

        m_frames.resize(m_size);
        m_timestamps.resize(m_size);

        m_write = 0;
        m_count = 0;
    }

    void reset(int size)
    {
        m_size = (size <= 0) ? 1 : size;
        reset();
    }

    int32_t size() const
    {
        return (m_count >= m_size) ? m_size : m_count;
    }

    T insert(T const& in, uint64_t timestamp)
    {
        T old = m_frames[m_write];

        m_frames[m_write] = in;
        m_timestamps[m_write] = timestamp;

        m_write = (m_write + 1) % m_size;
        m_count = (m_count + 1);

        return old;
    }

    int get(int32_t stamp, T& out, uint64_t& t, int32_t& s) const
    {
        int32_t base = size();
        int32_t index = (stamp < 0) ? (base + stamp) : (base + stamp - m_count);

        if (index < 0) { return -1; }
        if (index >= base) { return 1; }

        int32_t slot = (m_write + index) % base;

        out = m_frames[slot];
        t = m_timestamps[slot];
        s = index + m_count - base;
        return 0;
    }

    int get(uint64_t timestamp, int time_preference, bool tiebreak_right, T& out, uint64_t& t, int32_t& s) const
    {
        int32_t base = size();
        int32_t index = find_index(timestamp, time_preference, tiebreak_right, base);

        if (index < 0) { return 1; }

        out = m_frames[index];
        t = m_timestamps[index];
        s = m_count - base + ((base + index - m_write) % base);
        return 0;
    }

    T at(int32_t index) const
    {
        return m_frames[index];
    }
};
