import {Injectable} from '@angular/core';
import {HttpClient} from "@angular/common/http";
import {Observable} from "rxjs";
import {Registers} from "../models/registers";
import {InterruptDescriptor} from "../models/interrupt-descriptor";

@Injectable({
    providedIn: 'root'
})
export class BackendService {

    constructor(private http: HttpClient) {
    }

    getProcessorRegisters(): Observable<Registers> {
      return this.http.get<Registers>("/api/cpu/registers")
    }

    getKernelIdt(): Observable<InterruptDescriptor[]> {
        return this.http.get<InterruptDescriptor[]>("/api/kernel/idt")
    }
}
