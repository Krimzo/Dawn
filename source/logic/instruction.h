#pragma once


namespace faxdawn {
    enum class instruction_type
    {
        none = 0,
        move_to,
        set_memory,
        read_memory,
        call_function,
    };
    
    struct instruction
    {
    private:
        void move_to() noexcept;
        void set_memory() noexcept;
        void read_memory() noexcept;
        void call_function() noexcept;

    public:
        instruction_type type;
        void* first;
        void* second;

        instruction();
        instruction(instruction_type type, void* first, void* second);

        void execute() noexcept;
    };
}
