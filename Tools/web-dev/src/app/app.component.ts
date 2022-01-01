import {Component, OnInit} from '@angular/core';
import {BackendService} from "./services/backend.service";
import {Registers} from "./models/registers";
import {InterruptDescriptor} from "./models/interrupt-descriptor";

@Component({
    selector: 'app-root',
    templateUrl: './app.component.html',
    styleUrls: ['./app.component.scss'],
    providers: [BackendService]
})
export class AppComponent implements OnInit {
    public registers: Registers | undefined;
    public idt: InterruptDescriptor[] | undefined;

    constructor(private backendService: BackendService) {
    }

    ngOnInit(): void {
        this.backendService.getProcessorRegisters().subscribe(res => this.registers = res);
        this.backendService.getKernelIdt().subscribe(res => this.idt = res);
    }
}
