import {Pipe, PipeTransform} from '@angular/core';

@Pipe({
    name: 'hex'
})
export class HexPipe implements PipeTransform {
    transform(value: number | undefined, ...args: unknown[]): string {
        if(typeof value == "number") {
            return value.toString(16)
        }
        return "Unknown"
    }
}
