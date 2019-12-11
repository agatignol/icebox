import enum
import inspect
import os
import sys

# voodoo magic to attach dynamic properties to a single class instance
def _attach_dynamic_property(instance, name, propr):
    class_name = instance.__class__.__name__ + '_'
    child_class = type(class_name, (instance.__class__,), {name: propr})
    instance.__class__ = child_class

class Registers:
    def __init__(self, regs, read, write):
        for name, idx in regs():
            def get_property(idx):
                fget = lambda _: read(idx)
                fset = lambda _, arg: write(idx, arg)
                return property(fget, fset)
            _attach_dynamic_property(self, name, get_property(idx))

class Flags:
    def __init__(self, dict):
        for k, v in dict.items():
            setattr(self, k, v)

kFlags_x86 = Flags({"is_x86": True,  "is_x64": False})
kFlags_x64 = Flags({"is_x86": False, "is_x64": True})

class Symbols:
    def __init__(self, proc):
        self.proc = proc

    def address(self, name):
        module, symbol = name.split("!")
        return _icebox.symbols_address(self.proc, module, symbol)

    def struc_names(self, module):
        return _icebox.symbols_struc_names(self.proc, module)

    def struc_size(self, name):
        module, struc_name = name.split("!")
        return _icebox.symbols_struc_size(self.proc, module, struc_name)

    def struc_members(self, name):
        module, struc = name.split("!")
        return _icebox.symbols_struc_members(self.proc, module, struc)

    def member_offset(self, name):
        module, struc = name.split("!")
        struc_name, struc_member = struc.split("::")
        return _icebox.symbols_member_offset(self.proc, module, struc_name, struc_member)

    def string(self, ptr):
        return _icebox.symbols_string(self.proc, ptr)

class Process:
    def __init__(self, proc):
        self.proc = proc
        self.symbols = Symbols(proc)

    def name(self):
        return _icebox.process_name(self.proc)

    def is_valid(self):
        return _icebox.process_is_valid(self.proc)

    def pid(self):
        return _icebox.process_pid(self.proc)

    def flags(self):
        return Flags(_icebox.process_flags(self.proc))

    def join(self, mode):
        if mode != "kernel" and mode != "user":
            raise BaseException("invalid join mode")

        return _icebox.process_join(self.proc, mode)

    def parent(self):
        ret = _icebox.process_parent(self.proc)
        return Process(ret) if ret else None

class Callback:
    def __init__(self, bpid, callback):
        self.bpid = bpid
        self.callback = callback

class Processes:
    def __init__(self):
        pass

    def list_all(self):
        for x in _icebox.process_list():
            yield Process(x)

    def current(self):
        return Process(_icebox.process_current())

    def find_name(self, name, flags):
        for p in self.list_all():
            got_name = os.path.basename(p.name())
            if got_name != name:
                continue

            got_flags = p.flags()
            if flags.is_x64 and not got_flags.is_x64:
                continue

            if flags.is_x86 and not got_flags.is_x86:
                continue

            return p
        return None

    def find_pid(self, pid):
        for p in self.list_all():
            if p.pid() == pid:
                return p
        return None

    def wait(self, name, flags):
        return Process(_icebox.process_wait(name, flags))

    def break_on_create(self, callback):
        fproc = lambda proc: callback(Process(proc))
        bpid = _icebox.process_listen_create(fproc)
        return Callback(bpid, fproc)

    def break_on_delete(self, callback):
        fproc = lambda proc: callback(Process(proc))
        bpid = _icebox.process_listen_delete(fproc)
        return Callback(bpid, fproc)

class Thread:
    def __init__(self, thread):
        self.thread = thread

    def process(self):
        return Process(_icebox.thread_process(self.thread))

    def program_counter(self):
        return _icebox.thread_program_counter(self.thread)

    def tid(self):
        return _icebox.thread_tid(self.thread)

class Threads:
    def __init__(self):
        pass

    def list_all(self, proc):
        for x in _icebox.thread_list(proc.proc):
            yield Thread(x)

    def current(self):
        return Thread(_icebox.thread_current())

    def break_on_create(self):
        fthread = lambda thread: callback(Thread(thread))
        bpid = _icebox.thread_listen_create(fthread)
        return Callback(bpid, fthread)

    def break_on_delete(self):
        fthread = lambda thread: callback(Thread(thread))
        bpid = _icebox.thread_listen_delete(fthread)
        return Callback(bpid, fthread)

class Memory:
    def __init__(self):
        pass

    def virtual_to_physical(self, proc, ptr):
        return _icebox.memory_virtual_to_physical(proc.proc, ptr)

    def read_virtual(self, buf, ptr):
        return _icebox.memory_read_virtual(buf, ptr)

    def read_virtual_with_dtb(self, buf, dtb, ptr):
        return _icebox.memory_read_virtual_with_dtb(buf, dtb, ptr)

    def read_physical(self, buf, ptr):
        return _icebox.memory_read_physical(buf, ptr)

class Vm:
    def __init__(self, name):
        curr = inspect.getsourcefile(lambda: 0)
        path = os.path.abspath(os.path.join(curr, ".."))
        sys.path.append(path)
        global _icebox
        import _icebox
        _icebox.attach(name)
        self.registers = Registers(_icebox.register_list, _icebox.register_read, _icebox.register_write)
        self.msr = Registers(_icebox.msr_list, _icebox.msr_read, _icebox.msr_write)
        self.threads = Threads()
        self.processes = Processes()
        self.memory = Memory()

    def detach(self):
        _icebox.detach()

    def resume(self):
        _icebox.resume()

    def pause(self):
        _icebox.pause()

    def step_once(self):
        _icebox.single_step()

    def wait(self):
        _icebox.wait()

    def break_on(self, name, ptr, callback):
        pass

    def break_on_process(self, name, proc, ptr, callback):
        pass

    def break_on_thread(self, name, thread, ptr, callback):
        pass

    def break_on_physical(self, name, phy, callback):
        pass

    def break_on_physical_process(self, name, proc, phy, callback):
        pass
