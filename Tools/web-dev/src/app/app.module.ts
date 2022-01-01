import {NgModule} from '@angular/core';
import {BrowserModule} from '@angular/platform-browser';

import {AppRoutingModule} from './app-routing.module';
import {AppComponent} from './app.component';
import {HttpClientModule} from "@angular/common/http";
import { InterruptDescriptorTableComponent } from './interrupt-descriptor-table/interrupt-descriptor-table.component';
import { HexPipe } from './hex.pipe';

@NgModule({
    declarations: [
        AppComponent,
        InterruptDescriptorTableComponent,
        HexPipe
    ],
    imports: [
        BrowserModule,
        AppRoutingModule,
        HttpClientModule
    ],
    providers: [],
    bootstrap: [AppComponent]
})
export class AppModule {
}
