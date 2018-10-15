/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package consoleordreavion;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 *
 * @author jerem
 */
public class Pivot {
    public static byte[] toPivot(Integer nombre){
       return ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN).putInt(nombre).array();
    }
    
    public static int toInt(byte [] donnee){
        byte[] resultat = new byte[donnee.length];
        for(int i = 0; i<donnee.length;i++){
            resultat[i] = donnee[donnee.length-(i+1)];
        }
        return ByteBuffer.wrap(resultat).getInt();
    }
}
