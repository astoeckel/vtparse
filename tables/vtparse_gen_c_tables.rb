#!/usr/bin/env ruby

require_relative 'vtparse_tables'

class String
    def pad(len)
        self << (" " * (len - self.length))
    end
end

tar_hfile = "vtparse_table.h"
tar_cfile = "vtparse_table.c"

File.open(tar_hfile, "w") { |f|
    f.puts "/******************************************************************************"
    f.puts " * Note: This file was automatically generated. Please execute                *"
    f.puts " * tables/vtparse_gen_c_tables.rb to re-generate this file.                   *"
    f.puts " ******************************************************************************/"
    f.puts
    f.puts "#ifndef VTPARSE_VTPARSE_TABLE_H"
    f.puts "#define VTPARSE_VTPARSE_TABLE_H"
    f.puts
    f.puts "#ifdef __cplusplus"
    f.puts "extern \"C\" {"
    f.puts "#endif"
    f.puts
    f.puts "#ifndef VTPARSE_VTPARSE_H"
    f.puts
    f.puts "typedef enum {"
    $states_in_order.each_with_index { |state, i|
        f.puts "   VTPARSE_STATE_#{state.to_s.upcase} = #{i+1},"
    }
    f.puts "} vtparse_state_t;"
    f.puts
    f.puts "typedef enum {"
    $actions_in_order.each_with_index { |action, i|
        f.puts "   VTPARSE_ACTION_#{action.to_s.upcase} = #{i+1},"
    }
    f.puts "} vtparse_action_t;"
    f.puts
    f.puts
    f.puts "#endif"
    f.puts
    f.puts "typedef unsigned char vtparse_state_change_t;"
    f.puts
    f.puts "extern vtparse_state_change_t STATE_TABLE[#{$states_in_order.length}][256];"
    f.puts "extern vtparse_action_t ENTRY_ACTIONS[#{$states_in_order.length}];"
    f.puts "extern vtparse_action_t EXIT_ACTIONS[#{$states_in_order.length}];"
    f.puts "extern char *ACTION_NAMES[#{$actions_in_order.length+1}];"
    f.puts "extern char *STATE_NAMES[#{$states_in_order.length+1}];"
    f.puts
    f.puts "#ifdef __cplusplus"
    f.puts "}"
    f.puts "#endif"
    f.puts
    f.puts "#endif /* VTPARSE_VTPARSE_TABLE_H */"
    f.puts
}

puts "Wrote #{tar_hfile}"

File.open(tar_cfile, "w") { |f|
    f.puts "/******************************************************************************"
    f.puts " * Note: This file was automatically generated. Please execute                *"
    f.puts " * tables/vtparse_gen_c_tables.rb to re-generate this file.                   *"
    f.puts " ******************************************************************************/"
    f.puts
    f.puts '#include <vtparse/vtparse_table.h>'
    f.puts
    f.puts "char *ACTION_NAMES[] = {"
    f.puts "   \"<no action>\","
    $actions_in_order.each { |action|
        f.puts "   \"#{action.to_s.upcase}\","
    }
    f.puts "};"
    f.puts
    f.puts "char *STATE_NAMES[] = {"
    f.puts "   \"<no state>\","
    $states_in_order.each { |state|
        f.puts "   \"#{state.to_s}\","
    }
    f.puts "};"
    f.puts
    f.puts "vtparse_state_change_t STATE_TABLE[#{$states_in_order.length}][256] = {"
    $states_in_order.each_with_index { |state, i|
        f.puts "  {  /* VTPARSE_STATE_#{state.to_s.upcase} = #{i} */"
        $state_tables[state].each_with_index { |state_change, i|
            if not state_change
                f.puts "    0,"
            else
                (action,) = state_change.find_all { |s| s.kind_of?(Symbol) }
                (state,)  = state_change.find_all { |s| s.kind_of?(StateTransition) }
                action_str = action ? "VTPARSE_ACTION_#{action.to_s.upcase}" : "0"
                state_str =  state ? "VTPARSE_STATE_#{state.to_state.to_s}" : "0"
                f.puts "/*#{i.to_s.pad(3)}*/  #{action_str.pad(33)} | (#{state_str.pad(33)} << 4),"
            end
        }
        f.puts "  },"
    }

    f.puts "};"
    f.puts
    f.puts "vtparse_action_t ENTRY_ACTIONS[] = {"
    $states_in_order.each { |state|
        actions = $states[state]
        if actions[:on_entry]
            f.puts "   VTPARSE_ACTION_#{actions[:on_entry].to_s.upcase}, /* #{state} */"
        else
            f.puts "   0  /* none for #{state} */,"
        end
    }
    f.puts "};"
    f.puts
    f.puts "vtparse_action_t EXIT_ACTIONS[] = {"
    $states_in_order.each { |state|
        actions = $states[state]
        if actions[:on_exit]
            f.puts "   VTPARSE_ACTION_#{actions[:on_exit].to_s.upcase}, /* #{state} */"
        else
            f.puts "   0  /* none for #{state} */,"
        end
    }
    f.puts "};"
    f.puts
    f.puts "#ifdef __cplusplus"
    f.puts "}"
    f.puts "#endif"
}

puts "Wrote #{tar_cfile}"

