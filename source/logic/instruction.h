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
        void move_to() const;
        void set_memory() const;
        void read_memory() const;
        void call_function() const;

    public:
        instruction_type type;
        void* first;
        void* second;

        instruction();
        instruction(instruction_type type, void* first, void* second);

        void execute() const;
    };
}
